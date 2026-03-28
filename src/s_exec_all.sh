#!/bin/bash

set -e

sudo sysctl -w kernel.perf_event_paranoid=1

g++ -std=c++20 exp_time_cache.cpp -o exp_time_cache
./exp_time_cache

g++ -std=c++20 exp_ops.cpp -o exp_ops
./exp_ops

g++ -std=c++20 exp_pin.cpp -o exp_pin
chmod +x exp_pin
./s_exp_pin.sh

echo "Execução concluída com sucesso!"
