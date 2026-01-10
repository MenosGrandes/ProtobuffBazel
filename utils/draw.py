import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import sys

csv_file = sys.argv[1] if len(sys.argv) > 1 else "results.csv"

df = pd.read_csv(csv_file)

df = df[df['name'].str.endswith('_mean')].copy()

def extract_size(name: str) -> int:
    part = name.split("/")[-1]  # e.g., "2_mean"
    return int(''.join(filter(str.isdigit, part)))

df["size"] = df["name"].apply(extract_size)

benchmarks = sorted(set(s.split("/")[1][3:] for s in df['name'] if "/BM_" in s))
if not benchmarks:
    raise ValueError("No benchmarks with 'BM_' found in CSV.")

# --- Prepare merged dataframe ---
merged = df[['size']].drop_duplicates().sort_values('size')
for bench in benchmarks:
    bench_df = (
        df[df["name"].str.contains(f"/BM_{bench}/")]
        [['size', 'real_time']]
        .rename(columns={'real_time': bench})
    )
    merged = pd.merge(merged, bench_df, on='size', how='left')

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

fig_times = go.Figure()

for bench in benchmarks:
    fig_times.add_trace(
        go.Scatter(
            x=merged['size'],
            y=merged[bench],
            mode='lines+markers',
            name=bench,
            hovertemplate=
                f"<b>{bench}</b><br>" +
                "Size: %{x}<br>" +
                "Time: %{customdata}<extra></extra>",
            customdata=[format_time_ns(v) for v in merged[bench]]
        )
    )

fig_times.update_layout(
    title="All benchmark comparisons (mean)",
    xaxis_title="Message size (objects)",
    yaxis_title="Time",
    xaxis_type="log",
    yaxis_type="log",
    xaxis=dict(dtick=1),
    template="plotly_white",
    legend_title="Benchmark"
)

fig_speed = make_subplots(
    rows=len(benchmarks),
    cols=1,
    shared_xaxes=True,
    subplot_titles=[f"Speedups using {b} as base" for b in benchmarks]
)

for row, base_bench in enumerate(benchmarks, start=1):
    for other_bench in benchmarks:
        if other_bench == base_bench:
            continue

        speedup = (merged[base_bench] - merged[other_bench]) / merged[other_bench] * 100

        fig_speed.add_trace(
            go.Scatter(
                x=merged['size'],
                y=speedup,
                mode='lines+markers',
                name=f"{other_bench} vs {base_bench}",
                hovertemplate=
                    f"<b>{other_bench} vs {base_bench}</b><br>" +
                    "Size: %{x}<br>" +
                    "Speedup: %{y:.2f} %<extra></extra>",
            ),
            row=row,
            col=1
        )

    fig_speed.add_hline(y=0, line_dash="dash", row=row, col=1)

fig_speed.update_layout(
    height=350 * len(benchmarks),
    xaxis_type="log",
    xaxis=dict(dtick=1),
    yaxis_title="Speedup (%)",
    xaxis_title="Message size (objects)",
    template="plotly_white",
    showlegend=True
)

# ============================================================
# SAVE INTERACTIVE FIGURES
# ============================================================

fig_times.write_html("benchmarks_times.html")
fig_speed.write_html("benchmarks_speedups.html")

# Optional: also show them
fig_times.show()
fig_speed.show()
