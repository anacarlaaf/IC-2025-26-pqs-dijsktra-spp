#!/bin/bash
# ./exp_pin.sh
# chmod +x exp_pin.sh

PIN="/home/anacarla/Documents/pin-external-4.2-99776-g21d818fa2-gcc-linux/pin"
TOOL="/home/anacarla/Documents/pin-external-4.2-99776-g21d818fa2-gcc-linux/source/tools/ManualExamples/obj-intel64/dijkstra_opcodes.so"
programa="/home/anacarla/Documents/Repos/IC-2025-26-pqs-dijsktra-spp/src/exp_pin"
arquivo_inputs="in_pin_graphs.txt"
arquivo_filas="in_pin_filas.txt"
output_csv="/home/anacarla/Documents/Repos/IC-2025-26-pqs-dijsktra-spp/data/outs/all_pin.csv"

while IFS= read -r input || [[ -n "$input" ]]; do
    [[ -z "$input" ]] && continue

    while IFS= read -r fila || [[ -n "$fila" ]]; do
        [[ -z "$fila" ]] && continue

        echo "Executando: input=$input fila=$fila"

        # Roda o Pin capturando stdout do exp_pin; stderr (resumo do Pintool) vai pro terminal
        saida=$($PIN \
            -t  "$TOOL"          \
            -input "$input"      \
            -fila  "$fila"       \
            -o     "$output_csv" \
            -- "$programa" "$input" "$fila" 2>/dev/null)

        # Extrai n, m, c da saída do exp_pin
        n=$(echo "$saida" | grep "Vértices"   | awk '{print $2}')
        m=$(echo "$saida" | grep "Arestas"    | awk '{print $2}')
        c=$(echo "$saida" | grep "Maior peso" | awk '{print $3}')

        echo "  n=$n  m=$m  c=$c"

        # O Pintool já gravou a última linha no CSV como:
        #   input,fila,MOV,ADD,...
        # Queremos transformar em:
        #   input,fila,n,m,c,MOV,ADD,...

        # Separa tudo menos a última linha
        corpo=$(head -n -1 "$output_csv")
        ultima=$(tail -n 1  "$output_csv")

        prefixo=$(echo "$ultima" | cut -d',' -f1,2)
        metricas=$(echo "$ultima" | cut -d',' -f3-)

        # Na primeira execução (2 linhas = cabeçalho + 1 dado), atualiza cabeçalho também
        num_linhas=$(wc -l < "$output_csv")
        if [ "$num_linhas" -eq 2 ]; then
            cab_prefixo=$(echo "$corpo" | cut -d',' -f1,2)
            cab_metricas=$(echo "$corpo" | cut -d',' -f3-)
            printf '%s\n' "${cab_prefixo},n,m,c,${cab_metricas}" > "$output_csv"
        else
            printf '%s\n' "$corpo" > "$output_csv"
        fi

        # Acrescenta linha de dados com n,m,c
        printf '%s\n' "${prefixo},${n},${m},${c},${metricas}" >> "$output_csv"

        echo "  CSV atualizado."

    done < "$arquivo_filas"
done < "$arquivo_inputs"