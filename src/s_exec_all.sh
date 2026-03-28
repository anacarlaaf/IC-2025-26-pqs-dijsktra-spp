#!/bin/bash

set -e

sudo sysctl -w kernel.perf_event_paranoid=1

rm -f ../data/outs/all_mem.csv

g++ -std=c++20 exp_mem.cpp -o exp_mem
./s_exp_mem.sh

g++ -std=c++20 exp_time_cache.cpp -o exp_time_cache
./exp_time_cache


echo "Execução concluída com sucesso!"
echo -e "\a"