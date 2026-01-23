import os

def transformar_grafo(input_file, output_file):
    """
    Transforma um arquivo do formato DIMACS para o formato simplificado.
    """
    
    # Verificar se o arquivo de entrada existe
    if not os.path.exists(input_file):
        print(f"ERRO: Arquivo '{input_file}' não encontrado!")
        print(f"Diretório atual: {os.getcwd()}")
        print(f"Arquivos no diretório: {os.listdir('.')}")
        return
    
    try:
        with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
            primeiro_p = True
            contador_arestas = 0
            
            for linha_num, linha in enumerate(infile, 1):
                linha = linha.strip()
                if not linha or linha.startswith('c'):
                    continue
                
                if linha.startswith('p') and primeiro_p:
                    partes = linha.split()
                    if len(partes) >= 4:
                        outfile.write(f"{partes[2]} {partes[3]}\n")
                        primeiro_p = False
                        print(f"Informações do grafo: {partes[2]} nós, {partes[3]} arestas")
                
                elif linha.startswith('a'):
                    partes = linha.split()
                    if len(partes) >= 4:
                        outfile.write(f"{partes[1]} {partes[2]} {partes[3]}\n")
                        contador_arestas += 1
            
        print(f"✓ Arquivo salvo com sucesso: {output_file}")
        print(f"✓ Total de arestas processadas: {contador_arestas}")
        
    except Exception as e:
        print(f"ERRO ao processar arquivo: {e}")

grafos = ["NE", "CAL"]

for g in grafos:
    # Verificar se o arquivo existe
    arquivo_entrada = "../data/grs/USA-road-d."+g+".gr"
    if os.path.exists(arquivo_entrada):

        # Ler as primeiras linhas para verificar formato
        try:
            with open(arquivo_entrada, 'r') as f:
                for i in range(5):
                    linha = f.readline()
        except Exception as e:
            print(f"  ERRO ao ler arquivo: {e}")
    else:
        print(f"✗ Arquivo '{arquivo_entrada}' NÃO encontrado!")

    # Executar transformação
    print("\n=== INICIANDO TRANSFORMAÇÃO ===")
    transformar_grafo(arquivo_entrada, "../data/txts/"+g+".txt")