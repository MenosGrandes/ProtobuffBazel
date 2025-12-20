#!/bin/bash
set -e

BENCH_BINARY="$1"
shift  # remove first argument

# Bazel runfiles directory
RUNFILES_DIR="${RUNFILES_DIR:-$0.runfiles}"

# Construct path to draw.py
PYTHON_SCRIPT="$RUNFILES_DIR/_main/utils/draw.py"

if [[ ! -f "$PYTHON_SCRIPT" ]]; then
    echo "Error: Python script not found: $PYTHON_SCRIPT" >&2
    exit 1
fi

if [[ ! -f "$BENCH_BINARY" ]]; then
    echo "Error: benchmark binary not found: $BENCH_BINARY" >&2
    exit 1
fi

"$BENCH_BINARY" --benchmark_min_time=2 \
  --benchmark_repetitions=5 \
  --benchmark_report_aggregates_only=true \
  --benchmark_format=csv > results.csv
python3 "$PYTHON_SCRIPT" ParseFromString_NoArena ParseFromString_Arena
