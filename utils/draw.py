import pandas as pd
import matplotlib.pyplot as plt
import mplcursors
import sys

# --- Command-line arguments ---
if len(sys.argv) < 3:
    print("Usage: python draw.py <Benchmark1> <Benchmark2> [csv_file]")
    print("Example: python draw.py StdStringConcat CordConcat results.csv")
    sys.exit(1)

bench1_str = sys.argv[1]
bench2_str = sys.argv[2]
csv_file = sys.argv[3] if len(sys.argv) > 3 else "results.csv"

# --- Load CSV ---
df = pd.read_csv(csv_file)

# Keep only _mean rows
df = df[df['name'].str.endswith('_mean')].copy()

# Extract numeric size from benchmark name
def extract_size(name: str) -> int:
    part = name.split("/")[-1]  # e.g., "2_mean"
    return int(''.join(filter(str.isdigit, part)))

df["size"] = df["name"].apply(extract_size)

# Filter benchmarks
bench1 = df[df["name"].str.contains(f"/BM_{bench1_str}/")]
bench2 = df[df["name"].str.contains(f"/BM_{bench2_str}/")]

if bench1.empty:
    available = ", ".join(sorted(set(s.split("/")[1][3:] for s in df['name'])))
    raise ValueError(f"No benchmark matching '{bench1_str}' found. Available: {available}")
if bench2.empty:
    available = ", ".join(sorted(set(s.split("/")[1][3:] for s in df['name'])))
    raise ValueError(f"No benchmark matching '{bench2_str}' found. Available: {available}")

# Merge on size to align datasets
merged = pd.merge(
    bench1[['size','real_time']],
    bench2[['size','real_time']],
    on='size',
    suffixes=(f'_{bench1_str}', f'_{bench2_str}')
)

# Compute percentage speedup
merged['percent_speedup'] = (merged[f'real_time_{bench1_str}'] - merged[f'real_time_{bench2_str}']) \
                             / merged[f'real_time_{bench2_str}'] * 100

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

# --- Plot ---
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12,10), sharex=True)
plt.subplots_adjust(hspace=0.3)

# Top: raw times
line1, = ax1.plot(merged['size'], merged[f'real_time_{bench1_str}'], 'o-', label=bench1_str)
line2, = ax1.plot(merged['size'], merged[f'real_time_{bench2_str}'], 's-', label=bench2_str)
ax1.set_xscale('log', base=2)
ax1.set_yscale('log')
ax1.set_ylabel('Time')
ax1.set_title(f'Benchmark: {bench1_str} vs {bench2_str}')
ax1.grid(True, which='both', ls='--', alpha=0.5)
ax1.legend()

# Bottom: percentage speedup
line3, = ax2.plot(merged['size'], merged['percent_speedup'], 'o-', color='green', label=f'{bench2_str} faster (%)')
ax2.axhline(0, color='black', linestyle='--', linewidth=1)
ax2.set_xscale('log', base=2)
ax2.set_xlabel('Message size (objects)')
ax2.set_ylabel('Speedup (%)')
ax2.set_title('Percentage speedup per message size')
ax2.grid(True, which='both', ls='--', alpha=0.5)
ax2.legend()

# --- Interactive hover tooltips ---
cursor1 = mplcursors.cursor([line1, line2], hover=True)
cursor1.connect("add", lambda sel: sel.annotation.set_text(
    f"{sel.artist.get_label()}\nSize: {int(merged['size'].iloc[int(sel.index)])}\n"
    f"Time: {format_time_ns(sel.artist.get_ydata()[int(sel.index)])}"
))

cursor2 = mplcursors.cursor(line3, hover=True)
cursor2.connect("add", lambda sel: sel.annotation.set_text(
    f"Size: {int(merged['size'].iloc[int(sel.index)])}\n"
    f"Speedup: {sel.artist.get_ydata()[int(sel.index)]:.2f} %"
))

plt.show()
