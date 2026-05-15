import argparse
import sys

import numpy as np
import pandas as pd
from scipy import stats


GROUP_COLS = ["nome", "fila"]
NON_METRIC_COLS = {"nome", "n", "m", "c", "fila"}


def confidence_interval(series: pd.Series, confidence: float):
    """Retorna (média, half_width, low, high) usando a distribuição t de Student."""
    data = series.dropna().to_numpy(dtype=float)
    n = data.size
    mean = data.mean() if n else np.nan
    if n < 2:
        return mean, np.nan, np.nan, np.nan
    sem = stats.sem(data)
    half = sem * stats.t.ppf((1 + confidence) / 2.0, df=n - 1)
    return mean, half, mean - half, mean + half


def compute(df: pd.DataFrame, confidence: float) -> pd.DataFrame:
    metric_cols = [c for c in df.columns if c not in NON_METRIC_COLS]

    rows = []
    for keys, group in df.groupby(GROUP_COLS, sort=False):
        row = dict(zip(GROUP_COLS, keys))
        row["n_amostras"] = len(group)
        for col in metric_cols:
            mean, half, low, high = confidence_interval(group[col], confidence)
            row[f"{col}_media"] = mean
            row[f"{col}_ic_low"] = low
            row[f"{col}_ic_high"] = high
            row[f"{col}_ic_half"] = half
            row[f"{col}_ic_rel_pct"] = (half / mean * 100.0) if mean else np.nan
        rows.append(row)

    return pd.DataFrame(rows)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("arquivo", help="Caminho para o CSV de entrada (separado por espaços)")
    parser.add_argument("--confianca", type=float, default=0.95,
                        help="Nível de confiança (padrão: 0.95)")
    parser.add_argument("--saida", default=None,
                        help="CSV de saída (padrão: stdout)")
    args = parser.parse_args()

    if not 0 < args.confianca < 1:
        parser.error("--confianca deve estar entre 0 e 1 (ex.: 0.95)")

    df = pd.read_csv(args.arquivo, sep=r"\s+", engine="python")

    missing = [c for c in GROUP_COLS if c not in df.columns]
    if missing:
        parser.error(f"Colunas obrigatórias ausentes: {missing}")

    result = compute(df, args.confianca)

    if args.saida:
        result.to_csv(args.saida, index=False)
        print(f"Resultado salvo em {args.saida} "
              f"({len(result)} grupos, confiança {args.confianca:.0%}).",
              file=sys.stderr)
    else:
        with pd.option_context("display.max_columns", None,
                               "display.width", None,
                               "display.float_format", "{:.4f}".format):
            print(result.to_string(index=False))

    return 0


if __name__ == "__main__":
    sys.exit(main())
