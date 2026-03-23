#!/bin/bash

set -e

g++ -std=c++20 exp_mem.cpp -o exp_mem
./s_exp_mem.sh

./s_exp_pin.sh

g++ -std=c++20 exp_time_cache.cpp -o exp_time_cache
./exp_time_cache


echo "Execução concluída com sucesso!"
echo -e "\a"