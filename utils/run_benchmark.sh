#!/bin/bash
set -e

spinner() {
    local pid=$1
    local delay=0.1
    local spinstr='|/-\'
    while kill -0 "$pid" 2>/dev/null; do
        local temp=${spinstr#?}
        printf " [%c]  " "$spinstr"
        spinstr=$temp${spinstr%"$temp"}
        sleep $delay
        printf "\b\b\b\b\b\b"
    done
    printf "    \b\b\b\b"
}


BENCH_BINARY="$1"
shift  # remove first argument

# Bazel runfiles directory
RUNFILES_DIR="${RUNFILES_DIR:-$0.runfiles}"
echo "RUNFILES_DIR is: $RUNFILES_DIR"
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

"$BENCH_BINARY" --benchmark_min_time=2s \
  --benchmark_repetitions=5 \
  --benchmark_report_aggregates_only=true \
  --benchmark_format=csv > "results.csv" &

pid=$!
spinner $pid

# Wait for command to finish
wait $pid
exit_code=$?

echo "Done: exit code $exit_code"
python3 "$PYTHON_SCRIPT" "$@"
