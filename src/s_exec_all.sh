#!/bin/bash

set -e

g++ -std=c++20 exp_time_cache.cpp -o exp_time_cache
./exp_time_cache

g++ -std=c++20 exp_ops.cpp -o exp_ops
./exp_ops

g++ -std=c++20 exp_mem.cpp -o exp_mem
./exp_mem.sh

echo "Execução concluída com sucesso!"
echo -e "\a"