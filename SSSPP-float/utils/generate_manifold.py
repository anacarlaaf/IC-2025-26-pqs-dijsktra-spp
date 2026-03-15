"""
Gerador de manifold (Swiss Roll e outros) com exportação de grafo para ISOMAP/Dijkstra.

Uso:
    python generate_manifold_graph.py [opções]

Exemplos:
    python generate_manifold_graph.py --manifold swiss_roll --n_points 500 --k 7 --output grafo.txt
    python generate_manifold_graph.py --manifold sphere --n_points 300 --k 5 --min_dist 0.1 --max_dist 5.0
    python generate_manifold_graph.py --manifold torus --n_points 400 --k 6 --output torus_grafo.txt
"""

import argparse
import numpy as np
from sklearn.neighbors import NearestNeighbors
import scipy.sparse.csgraph as csgraph
import scipy.sparse as sp
import sys


# ──────────────────────────────────────────────
#  Geradores de manifold
# ──────────────────────────────────────────────

def make_swiss_roll(n_points: int, noise: float = 0.0,
                    min_dist: float = 0.0, max_dist: float = np.inf,
                    random_state: int = 42) -> np.ndarray:
    """Swiss Roll 3‑D."""
    rng = np.random.RandomState(random_state)
    t = 1.5 * np.pi * (1 + 2 * rng.rand(n_points))
    height = rng.rand(n_points)
    x = t * np.cos(t)
    y = height * 10
    z = t * np.sin(t)
    pts = np.column_stack([x, y, z])
    if noise > 0:
        pts += rng.randn(*pts.shape) * noise
    return pts


def make_sphere(n_points: int, noise: float = 0.0,
                min_dist: float = 0.0, max_dist: float = np.inf,
                random_state: int = 42) -> np.ndarray:
    """Esfera unitária em 3‑D (distribuição uniforme)."""
    rng = np.random.RandomState(random_state)
    pts = rng.randn(n_points, 3)
    pts /= np.linalg.norm(pts, axis=1, keepdims=True)
    if noise > 0:
        pts += rng.randn(*pts.shape) * noise
    return pts


def make_torus(n_points: int, noise: float = 0.0,
               min_dist: float = 0.0, max_dist: float = np.inf,
               random_state: int = 42,
               R: float = 3.0, r: float = 1.0) -> np.ndarray:
    """Torus 3‑D com raio maior R e raio menor r."""
    rng = np.random.RandomState(random_state)
    theta = 2 * np.pi * rng.rand(n_points)
    phi   = 2 * np.pi * rng.rand(n_points)
    x = (R + r * np.cos(phi)) * np.cos(theta)
    y = (R + r * np.cos(phi)) * np.sin(theta)
    z = r * np.sin(phi)
    pts = np.column_stack([x, y, z])
    if noise > 0:
        pts += rng.randn(*pts.shape) * noise
    return pts


def make_s_curve(n_points: int, noise: float = 0.0,
                 min_dist: float = 0.0, max_dist: float = np.inf,
                 random_state: int = 42) -> np.ndarray:
    """S‑Curve 3‑D."""
    rng = np.random.RandomState(random_state)
    t = 3 * np.pi * (rng.rand(n_points) - 0.5)
    x = np.sin(t)
    y = 2.0 * rng.rand(n_points)
    z = np.sign(t) * (np.cos(t) - 1)
    pts = np.column_stack([x, y, z])
    if noise > 0:
        pts += rng.randn(*pts.shape) * noise
    return pts


MANIFOLDS = {
    "swiss_roll": make_swiss_roll,
    "sphere":     make_sphere,
    "torus":      make_torus,
    "s_curve":    make_s_curve,
}


# ──────────────────────────────────────────────
#  Construção do grafo k‑NN com conectividade garantida
# ──────────────────────────────────────────────

def build_connected_knn_graph(points: np.ndarray, k: int,
                               min_dist: float = 0.0,
                               max_dist: float = np.inf):
    """
    Constrói grafo k‑NN não‑dirigido com arestas filtradas por [min_dist, max_dist].
    Se o grafo resultante for desconexo, adiciona arestas mínimas entre componentes
    até que o grafo seja completamente conectado.

    Retorna lista de tuplas (v1, v2, peso) com índices 0‑based.
    """
    n = len(points)
    # k+1 porque o próprio ponto é retornado como vizinho 0
    k_query = min(k + 1, n)
    nbrs = NearestNeighbors(n_neighbors=k_query, algorithm="ball_tree").fit(points)
    distances, indices = nbrs.kneighbors(points)

    edges = {}  # (min_i, max_j) → peso  (evita duplicatas)

    for i in range(n):
        for j_idx in range(1, k_query):  # ignora o próprio ponto (idx 0)
            j = indices[i, j_idx]
            d = distances[i, j_idx]
            if min_dist <= d <= max_dist:
                key = (min(i, j), max(i, j))
                if key not in edges:
                    edges[key] = d

    # ── Verificar conectividade e corrigir ──────────────────────────────
    def build_sparse(edge_dict, n_nodes):
        rows, cols, data = [], [], []
        for (u, v), w in edge_dict.items():
            rows += [u, v]; cols += [v, u]; data += [w, w]
        return sp.csr_matrix((data, (rows, cols)), shape=(n_nodes, n_nodes))

    iteration = 0
    max_iterations = n  # limite de segurança
    while iteration < max_iterations:
        mat = build_sparse(edges, n)
        n_comp, labels = csgraph.connected_components(mat, directed=False)
        if n_comp == 1:
            break

        # Para cada par de componentes, encontra a aresta mais curta e a adiciona
        comp_ids = np.unique(labels)
        added = 0
        for ci in comp_ids:
            for cj in comp_ids:
                if cj <= ci:
                    continue
                pts_i = np.where(labels == ci)[0]
                pts_j = np.where(labels == cj)[0]
                # distância mínima entre os dois conjuntos
                sub_i = points[pts_i]
                sub_j = points[pts_j]
                # NearestNeighbors entre os dois subconjuntos
                tmp = NearestNeighbors(n_neighbors=1).fit(sub_j)
                dists_ij, idx_ij = tmp.kneighbors(sub_i)
                flat_idx = np.argmin(dists_ij)
                best_d = dists_ij[flat_idx, 0]
                u = pts_i[flat_idx]
                v = pts_j[idx_ij[flat_idx, 0]]
                key = (min(u, v), max(u, v))
                if key not in edges:
                    edges[key] = best_d
                    added += 1
        if added == 0:
            # Não encontrou novas arestas — evita loop infinito
            break
        iteration += 1

    return list(edges.items())  # [((v1, v2), peso), ...]


# ──────────────────────────────────────────────
#  Exportação
# ──────────────────────────────────────────────

def export_graph(edge_list, n_vertices: int, output_path: str):
    """Exporta grafo no formato:
        qtd_vertices qtd_arestas
        v1 v2 peso
    Índices começam em 0.
    """
    n_edges = len(edge_list)
    with open(output_path, "w") as f:
        f.write(f"{n_vertices} {n_edges}\n")
        for (v1, v2), w in edge_list:
            f.write(f"{v1} {v2} {w:.10f}\n")
    print(f"[✓] Grafo exportado → {output_path}")
    print(f"    Vértices : {n_vertices}")
    print(f"    Arestas  : {n_edges}")


# ──────────────────────────────────────────────
#  CLI
# ──────────────────────────────────────────────

def parse_args():
    parser = argparse.ArgumentParser(
        description="Gera manifold + grafo k‑NN conectado e exporta para .txt."
    )
    parser.add_argument("--manifold",  choices=list(MANIFOLDS.keys()),
                        default="swiss_roll",
                        help="Tipo de manifold (default: swiss_roll)")
    parser.add_argument("--n_points",  type=int,   default=300,
                        help="Número de pontos (default: 300)")
    parser.add_argument("--k",         type=int,   default=7,
                        help="Vizinhos mais próximos por ponto (default: 7)")
    parser.add_argument("--min_dist",  type=float, default=0.0,
                        help="Distância mínima de aresta (default: 0.0)")
    parser.add_argument("--max_dist",  type=float, default=float("inf"),
                        help="Distância máxima de aresta (default: inf)")
    parser.add_argument("--noise",     type=float, default=0.0,
                        help="Ruído gaussiano adicionado ao manifold (default: 0.0)")
    parser.add_argument("--seed",      type=int,   default=42,
                        help="Semente aleatória (default: 42)")
    parser.add_argument("--output",    type=str,   default="grafo.txt",
                        help="Arquivo de saída (default: grafo.txt)")
    return parser.parse_args()


def main():
    args = parse_args()

    print(f"[·] Gerando manifold '{args.manifold}' com {args.n_points} pontos...")
    gen_fn = MANIFOLDS[args.manifold]
    points = gen_fn(
        n_points=args.n_points,
        noise=args.noise,
        min_dist=args.min_dist,
        max_dist=args.max_dist,
        random_state=args.seed,
    )
    print(f"    Shape dos pontos: {points.shape}")

    print(f"[·] Construindo grafo k‑NN (k={args.k}, min_dist={args.min_dist}, max_dist={args.max_dist})...")
    edge_list = build_connected_knn_graph(
        points, k=args.k,
        min_dist=args.min_dist,
        max_dist=args.max_dist,
    )

    export_graph(edge_list, n_vertices=args.n_points, output_path=args.output)


if __name__ == "__main__":
    main()