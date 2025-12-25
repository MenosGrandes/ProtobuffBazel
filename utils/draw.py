import pandas as pd
import matplotlib.pyplot as plt
import mplcursors
import sys

# --- CSV file from command-line or default ---
csv_file = sys.argv[1] if len(sys.argv) > 1 else "results.csv"

# --- Load CSV ---
df = pd.read_csv(csv_file)

# Keep only _mean rows
df = df[df['name'].str.endswith('_mean')].copy()

# Extract numeric size from benchmark name
def extract_size(name: str) -> int:
    part = name.split("/")[-1]  # e.g., "2_mean"
    return int(''.join(filter(str.isdigit, part)))

df["size"] = df["name"].apply(extract_size)

# --- Find all unique benchmark types containing BM_ ---
benchmarks = sorted(set(s.split("/")[1][3:] for s in df['name'] if "/BM_" in s))
if not benchmarks:
    raise ValueError("No benchmarks with 'BM_' found in CSV.")

# --- Prepare a merged dataframe with all benchmarks on same size ---
merged = df[['size']].drop_duplicates().sort_values('size')
for bench in benchmarks:
    bench_df = df[df["name"].str.contains(f"/BM_{bench}/")][['size','real_time']].rename(
        columns={'real_time': bench})
    merged = pd.merge(merged, bench_df, on='size', how='left')

# --- Assign consistent colors ---
colors = plt.cm.tab10.colors  # 10 distinct colors
bench_colors = {bench: colors[i % len(colors)] for i, bench in enumerate(benchmarks)}

# --- Helper: format time ---
def format_time_ns(ns: float) -> str:
    if ns < 1e3:
        return f"{ns:.2f} ns"
    elif ns < 1e6:
        return f"{ns/1e3:.2f} µs"
    elif ns < 1e9:
        return f"{ns/1e6:.2f} ms"
    else:
        return f"{ns/1e9:.2f} s"

# --- Figure 1: Raw times ---
fig_times, ax_times = plt.subplots(figsize=(14,6))
lines_times = []
for bench in benchmarks:
    line, = ax_times.plot(merged['size'], merged[bench], 'o-', label=bench, color=bench_colors[bench])
    lines_times.append(line)

ax_times.set_xscale('log', base=2)
ax_times.set_yscale('log')
ax_times.set_xlabel('Message size (objects)')
ax_times.set_ylabel('Time')
ax_times.set_title('All benchmark comparisons (_mean)')
ax_times.grid(True, which='both', ls='--', alpha=0.5)
ax_times.legend()

cursor_times = mplcursors.cursor(lines_times, hover=True)
cursor_times.connect("add", lambda sel: sel.annotation.set_text(
    f"{sel.artist.get_label()}\nSize: {int(merged['size'].iloc[int(sel.index)])}\n"
    f"Time: {format_time_ns(sel.artist.get_ydata()[int(sel.index)])}"
))

# --- Figure 2: All speedups in subplots ---
n = len(benchmarks)
fig_speed, axes = plt.subplots(n, 1, figsize=(14, 4*n), sharex=True)
if n == 1:
    axes = [axes]  # ensure iterable

for i, base_bench in enumerate(benchmarks):
    ax = axes[i]
    lines_speed = []
    for other_bench in benchmarks:
        if other_bench == base_bench:
            continue
        speedup = (merged[base_bench] - merged[other_bench]) / merged[other_bench] * 100
        line, = ax.plot(merged['size'], speedup, 'o-', label=f'{other_bench} vs {base_bench}', color=bench_colors[other_bench])
        lines_speed.append(line)

    ax.axhline(0, color='black', linestyle='--', linewidth=1)
    ax.set_xscale('log', base=2)
    ax.set_ylabel('Speedup (%)')
    ax.set_title(f'Speedups using {base_bench} as base')
    ax.grid(True, which='both', ls='--', alpha=0.5)
    ax.legend()

    cursor_speed = mplcursors.cursor(lines_speed, hover=True)
    cursor_speed.connect("add", lambda sel: sel.annotation.set_text(
        f"{sel.artist.get_label()}\nSize: {int(merged['size'].iloc[int(sel.index)])}\n"
        f"Speedup: {sel.artist.get_ydata()[int(sel.index)]:.2f} %"
    ))

axes[-1].set_xlabel('Message size (objects)')
plt.tight_layout()

# --- Show both figures at once ---
plt.show()
