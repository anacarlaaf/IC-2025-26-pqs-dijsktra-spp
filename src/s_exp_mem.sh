#!/bin/bash

# ./exp_mem.sh
programa="./exp_mem"   # nome do executável
arquivo_inputs="in_mem_graphs.txt"
arquivo_filas="in_mem_filas.txt"

primeira_exec=1

while IFS= read -r input || [[ -n "$input" ]]; do
    # ignora linhas vazias
    [[ -z "$input" ]] && continue

    while IFS= read -r fila || [[ -n "$fila" ]]; do
        [[ -z "$fila" ]] && continue

        if [ $primeira_exec -eq 1 ]; then
            restart=1
            primeira_exec=0
        else
            restart=0
        fi

        echo "Executando: input=$input fila=$fila restart=$restart"

        $programa "$input" "$fila" "$restart"

    done < "$arquivo_filas"

done < "$arquivo_inputs"