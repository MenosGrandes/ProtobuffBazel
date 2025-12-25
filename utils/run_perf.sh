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

BENCH_BINARY=$1
shift
perf stat -e cpu-clock,cycles,instructions,cache-misses,cache-references "$BENCH_BINARY" "$@"


"$BENCH_BINARY" "$@" &
perf record -F 10000 -g -p $!

pid=$!
spinner $pid

# Wait for command to finish
wait $pid
exit_code=$?

echo "Done: exit code $exit_code"
