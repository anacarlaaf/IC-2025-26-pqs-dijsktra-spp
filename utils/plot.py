import matplotlib.pyplot as plt
import pandas as pd

def ler_dados(arquivo_entrada):
    """Lê os dados do arquivo e retorna um DataFrame"""
    dados = []
    filas = []
    
    with open(arquivo_entrada, 'r') as f:
        # Lê o cabeçalho
        cabecalho = f.readline().strip().split()
        filas = cabecalho[4:]
        
        # Lê as linhas de dados
        for linha in f:
            if linha.strip():  # Ignora linhas vazias
                valores = linha.strip().split()
                # Converte valores numéricos
                linha_dict = {}
                for i, valor in enumerate(valores):
                    if i in [1, 2, 3]:  # n, m, c são inteiros
                        linha_dict[cabecalho[i]] = int(valor)
                    elif i >= 4:  # Tempos são floats
                        linha_dict[cabecalho[i]] = float(valor)
                    else:  # Nome é string
                        linha_dict[cabecalho[i]] = valor
                dados.append(linha_dict)
    
    return pd.DataFrame(dados), filas


def plotar_grafico(df, pqs):
    df = df.sort_values('n').reset_index(drop=True)
    
    marcadores = ['o', 's', '^', 'D', 'v', 'p', '*', 'h', 'x', '+']
    
    plt.figure(figsize=(10, 6))
    for i, pq in enumerate(pqs):
        plt.plot(df['n'], df[pq], 
                 label=pq, 
                 linewidth=2,
                 marker=marcadores[i % len(marcadores)],
                 markersize=6)
    
    plt.xlabel('n (número de vértices)', fontsize=12)
    plt.ylabel('Tempo de execução', fontsize=12)
    plt.title('Tempos de execução por n', fontsize=14, fontweight='bold')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig('data/graphs/tn.png', dpi=300)
    plt.show()

def main():
    # Nome do arquivo de entrada
    arquivo_entrada = 'data/outs/all_time.csv'  # Altere para o nome do seu arquivo
    
    try:
        # Ler os dados
        res = ler_dados(arquivo_entrada)
        df = res[0]
        pqs = res[1]
        
        print("Dados lidos com sucesso!")
        
        # Plotar gráficos
        print(df.head())
        plotar_grafico(df, pqs)
        
    except FileNotFoundError:
        print(f"Erro: Arquivo '{arquivo_entrada}' não encontrado!")
        print("Por favor, crie um arquivo 'dados.txt' com o formato especificado.")

if __name__ == "__main__":
    main()