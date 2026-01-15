import matplotlib.pyplot as plt
import pandas as pd

def ler_dados(arquivo_entrada):
    """Lê os dados do arquivo e retorna um DataFrame"""
    dados = []
    
    with open(arquivo_entrada, 'r') as f:
        # Lê o cabeçalho
        cabecalho = f.readline().strip().split()
        
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
    
    return pd.DataFrame(dados)


def plotar_grafico(df):
    
    algoritmos = ['BINARY_HEAP', 'B_HEAP', 'RB_TREE', 'FIBONACCI_HEAP', 'DIAL', 'TWO_LV_BKT_HEAP', 'RADIX_HEAP']
    marcadores = ['o', 's', '^', 'D', 'v', '*', 'p', 'h', 'X', 'P', "Q", 'A']
    
    # Gráfico 1: Tempos por n
    plt.figure(figsize=(10, 6))
    for i, algoritmo in enumerate(algoritmos):
        plt.plot(df['n'], df[algoritmo], marker=marcadores[i % len(marcadores)], label=algoritmo, linewidth=2)
    
    plt.xlabel('n (número de vértices)', fontsize=12)
    plt.ylabel('Tempo de execução', fontsize=12)
    plt.title('Tempos de execução por n', fontsize=14, fontweight='bold')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig('data/graphs/tn.png', dpi=300)

def main():
    # Nome do arquivo de entrada
    arquivo_entrada = 'data/outs/time.csv'  # Altere para o nome do seu arquivo
    
    try:
        # Ler os dados
        df = ler_dados(arquivo_entrada)
        
        print("Dados lidos com sucesso!")
        
        # Plotar gráficos
        plotar_grafico(df)
        
    except FileNotFoundError:
        print(f"Erro: Arquivo '{arquivo_entrada}' não encontrado!")
        print("Por favor, crie um arquivo 'dados.txt' com o formato especificado.")

if __name__ == "__main__":
    main()