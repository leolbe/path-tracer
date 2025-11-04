#!/usr/bin/env bash
set -e
cmake --build build
result=$(stdbuf -oL ./build/path_tracer_2025 | tee /dev/tty)
output_path=$(echo "$result" | tail --lines 1)
kitty +kitten icat $output_path
