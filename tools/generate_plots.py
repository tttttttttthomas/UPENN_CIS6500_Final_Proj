#!/usr/bin/env python3
"""
Generate performance comparison plots for Flood Index project
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from pathlib import Path

# Set style
plt.style.use('seaborn-v0_8-darkgrid')
sns.set_palette("husl")

# Read data
data_file = Path(__file__).parent.parent / "build" / "benchmark_results.csv"
df = pd.read_csv(data_file)

# Create output directory
output_dir = Path(__file__).parent.parent / "plots"
output_dir.mkdir(exist_ok=True)

print(f"Reading data from: {data_file}")
print(f"Output directory: {output_dir}")
print(f"\nData shape: {df.shape}")
print(f"Indexes: {df['Index'].unique()}")
print(f"Workloads: {df['Workload'].unique()}\n")

# Define colors for each index
colors = {
    'k-d Tree': '#3498db',  # Blue
    'Z-order': '#e74c3c',   # Red
    'R*-tree': '#2ecc71',   # Green
    'Flood': '#f39c12'      # Orange (highlight)
}

# =====================================================================
# Figure 1: Index Size Comparison
# =====================================================================
def plot_index_size():
    plt.figure(figsize=(10, 6))
    
    # Get average index size per index (same for all workloads)
    size_data = df.groupby('Index')['IndexSize_MB'].first().sort_values()
    
    bars = plt.bar(range(len(size_data)), size_data.values, 
                   color=[colors[idx] for idx in size_data.index],
                   edgecolor='black', linewidth=1.2, alpha=0.85)
    
    # Highlight Flood
    flood_idx = list(size_data.index).index('Flood')
    bars[flood_idx].set_edgecolor('red')
    bars[flood_idx].set_linewidth(3)
    
    plt.xticks(range(len(size_data)), size_data.index, fontsize=12, fontweight='bold')
    plt.ylabel('Index Size (MB)', fontsize=13, fontweight='bold')
    plt.title('Index Size Comparison (50K points)', fontsize=15, fontweight='bold', pad=20)
    plt.grid(axis='y', alpha=0.3, linestyle='--')
    
    # Add value labels on bars
    for i, (idx, val) in enumerate(size_data.items()):
        plt.text(i, val + 0.15, f'{val:.2f}', ha='center', va='bottom', 
                fontsize=11, fontweight='bold')
        
        # Add percentage comparison to Flood
        if idx != 'Flood':
            flood_size = size_data['Flood']
            pct_diff = ((val - flood_size) / flood_size) * 100
            plt.text(i, val/2, f'+{pct_diff:.0f}%', ha='center', va='center',
                    fontsize=10, color='white', fontweight='bold',
                    bbox=dict(boxstyle='round', facecolor='black', alpha=0.7))
    
    plt.tight_layout()
    plt.savefig(output_dir / "1_index_size_comparison.png", dpi=300, bbox_inches='tight')
    print("✓ Generated: 1_index_size_comparison.png")
    plt.close()

# =====================================================================
# Figure 2: Build Time Comparison
# =====================================================================
def plot_build_time():
    plt.figure(figsize=(10, 6))
    
    # Get average build time per index
    build_data = df.groupby('Index')['BuildTime_ms'].mean().sort_values()
    
    bars = plt.bar(range(len(build_data)), build_data.values,
                   color=[colors[idx] for idx in build_data.index],
                   edgecolor='black', linewidth=1.2, alpha=0.85)
    
    # Highlight Flood
    flood_idx = list(build_data.index).index('Flood')
    bars[flood_idx].set_edgecolor('red')
    bars[flood_idx].set_linewidth(3)
    
    plt.xticks(range(len(build_data)), build_data.index, fontsize=12, fontweight='bold')
    plt.ylabel('Build Time (ms)', fontsize=13, fontweight='bold')
    plt.title('Build Time Comparison (50K points)', fontsize=15, fontweight='bold', pad=20)
    plt.grid(axis='y', alpha=0.3, linestyle='--')
    
    # Add value labels
    for i, (idx, val) in enumerate(build_data.items()):
        plt.text(i, val + 0.5, f'{val:.1f}', ha='center', va='bottom',
                fontsize=11, fontweight='bold')
    
    plt.tight_layout()
    plt.savefig(output_dir / "2_build_time_comparison.png", dpi=300, bbox_inches='tight')
    print("✓ Generated: 2_build_time_comparison.png")
    plt.close()

# =====================================================================
# Figure 3: Query Time by Workload
# =====================================================================
def plot_query_time_by_workload():
    fig, axes = plt.subplots(1, 3, figsize=(16, 5))
    
    workload_names = {
        'Workload_A_Spatial': 'Workload A\n(Spatial, 0.1%)',
        'Workload_B_Temporal': 'Workload B\n(Temporal, 0.5%)',
        'Workload_C_Mixed': 'Workload C\n(Mixed, 1%)'
    }
    
    for ax_idx, (workload, workload_label) in enumerate(workload_names.items()):
        ax = axes[ax_idx]
        
        # Get data for this workload
        workload_data = df[df['Workload'] == workload].set_index('Index')['AvgQueryTime_ms']
        workload_data = workload_data.sort_values()
        
        bars = ax.bar(range(len(workload_data)), workload_data.values,
                     color=[colors[idx] for idx in workload_data.index],
                     edgecolor='black', linewidth=1.2, alpha=0.85)
        
        # Highlight Flood
        if 'Flood' in workload_data.index:
            flood_idx = list(workload_data.index).index('Flood')
            bars[flood_idx].set_edgecolor('red')
            bars[flood_idx].set_linewidth(3)
        
        ax.set_xticks(range(len(workload_data)))
        ax.set_xticklabels(workload_data.index, fontsize=10, fontweight='bold', rotation=15)
        ax.set_ylabel('Query Time (ms)', fontsize=11, fontweight='bold')
        ax.set_title(workload_label, fontsize=12, fontweight='bold', pad=10)
        ax.grid(axis='y', alpha=0.3, linestyle='--')
        
        # Add value labels
        for i, (idx, val) in enumerate(workload_data.items()):
            ax.text(i, val + max(workload_data)*0.05, f'{val:.3f}',
                   ha='center', va='bottom', fontsize=9, fontweight='bold')
    
    fig.suptitle('Query Time Comparison by Workload', fontsize=16, fontweight='bold', y=1.02)
    plt.tight_layout()
    plt.savefig(output_dir / "3_query_time_by_workload.png", dpi=300, bbox_inches='tight')
    print("✓ Generated: 3_query_time_by_workload.png")
    plt.close()

# =====================================================================
# Figure 4: Average Query Time (Log Scale)
# =====================================================================
def plot_avg_query_time_log():
    plt.figure(figsize=(10, 6))
    
    # Calculate average query time across all workloads
    avg_query = df.groupby('Index')['AvgQueryTime_ms'].mean().sort_values()
    
    bars = plt.bar(range(len(avg_query)), avg_query.values,
                   color=[colors[idx] for idx in avg_query.index],
                   edgecolor='black', linewidth=1.2, alpha=0.85)
    
    # Highlight Flood
    flood_idx = list(avg_query.index).index('Flood')
    bars[flood_idx].set_edgecolor('red')
    bars[flood_idx].set_linewidth(3)
    
    plt.yscale('log')
    plt.xticks(range(len(avg_query)), avg_query.index, fontsize=12, fontweight='bold')
    plt.ylabel('Average Query Time (ms, log scale)', fontsize=13, fontweight='bold')
    plt.title('Average Query Time Comparison (All Workloads)', fontsize=15, fontweight='bold', pad=20)
    plt.grid(axis='y', alpha=0.3, linestyle='--', which='both')
    
    # Add value labels
    for i, (idx, val) in enumerate(avg_query.items()):
        plt.text(i, val * 1.3, f'{val:.3f}', ha='center', va='bottom',
                fontsize=11, fontweight='bold')
    
    plt.tight_layout()
    plt.savefig(output_dir / "4_avg_query_time_log.png", dpi=300, bbox_inches='tight')
    print("✓ Generated: 4_avg_query_time_log.png")
    plt.close()

# =====================================================================
# Figure 5: Query Time Percentiles (P95)
# =====================================================================
def plot_query_percentiles():
    fig, axes = plt.subplots(1, 2, figsize=(14, 6))
    
    # P95
    ax = axes[0]
    p95_data = df.groupby('Index')['P95QueryTime_ms'].mean().sort_values()
    bars = ax.bar(range(len(p95_data)), p95_data.values,
                  color=[colors[idx] for idx in p95_data.index],
                  edgecolor='black', linewidth=1.2, alpha=0.85)
    
    flood_idx = list(p95_data.index).index('Flood')
    bars[flood_idx].set_edgecolor('red')
    bars[flood_idx].set_linewidth(3)
    
    ax.set_xticks(range(len(p95_data)))
    ax.set_xticklabels(p95_data.index, fontsize=11, fontweight='bold')
    ax.set_ylabel('P95 Query Time (ms)', fontsize=12, fontweight='bold')
    ax.set_title('P95 Query Time', fontsize=13, fontweight='bold')
    ax.grid(axis='y', alpha=0.3, linestyle='--')
    
    for i, (idx, val) in enumerate(p95_data.items()):
        ax.text(i, val + max(p95_data)*0.05, f'{val:.3f}',
               ha='center', va='bottom', fontsize=10, fontweight='bold')
    
    # P99
    ax = axes[1]
    p99_data = df.groupby('Index')['P99QueryTime_ms'].mean().sort_values()
    bars = ax.bar(range(len(p99_data)), p99_data.values,
                  color=[colors[idx] for idx in p99_data.index],
                  edgecolor='black', linewidth=1.2, alpha=0.85)
    
    flood_idx = list(p99_data.index).index('Flood')
    bars[flood_idx].set_edgecolor('red')
    bars[flood_idx].set_linewidth(3)
    
    ax.set_xticks(range(len(p99_data)))
    ax.set_xticklabels(p99_data.index, fontsize=11, fontweight='bold')
    ax.set_ylabel('P99 Query Time (ms)', fontsize=12, fontweight='bold')
    ax.set_title('P99 Query Time', fontsize=13, fontweight='bold')
    ax.grid(axis='y', alpha=0.3, linestyle='--')
    
    for i, (idx, val) in enumerate(p99_data.items()):
        ax.text(i, val + max(p99_data)*0.05, f'{val:.3f}',
               ha='center', va='bottom', fontsize=10, fontweight='bold')
    
    fig.suptitle('Query Time Percentiles Comparison', fontsize=15, fontweight='bold', y=1.00)
    plt.tight_layout()
    plt.savefig(output_dir / "5_query_percentiles.png", dpi=300, bbox_inches='tight')
    print("✓ Generated: 5_query_percentiles.png")
    plt.close()

# =====================================================================
# Figure 6: Comprehensive Performance Radar Chart
# =====================================================================
def plot_radar_chart():
    from math import pi
    
    fig, ax = plt.subplots(figsize=(10, 10), subplot_kw=dict(projection='polar'))
    
    # Prepare data (normalize to 0-100 scale, lower is better for most metrics)
    # We'll invert so higher is better for visualization
    metrics = ['Index Size', 'Build Time', 'Avg Query\nTime', 'P95 Query\nTime', 'P99 Query\nTime']
    
    # Get average values
    avg_data = df.groupby('Index').agg({
        'IndexSize_MB': 'first',
        'BuildTime_ms': 'mean',
        'AvgQueryTime_ms': 'mean',
        'P95QueryTime_ms': 'mean',
        'P99QueryTime_ms': 'mean'
    })
    
    # Normalize (invert so higher is better): score = (max - value) / (max - min) * 100
    def normalize_inverted(series):
        return ((series.max() - series) / (series.max() - series.min())) * 100
    
    normalized = pd.DataFrame({
        'Index Size': normalize_inverted(avg_data['IndexSize_MB']),
        'Build Time': normalize_inverted(avg_data['BuildTime_ms']),
        'Avg Query Time': normalize_inverted(avg_data['AvgQueryTime_ms']),
        'P95 Query Time': normalize_inverted(avg_data['P95QueryTime_ms']),
        'P99 Query Time': normalize_inverted(avg_data['P99QueryTime_ms'])
    })
    
    # Number of variables
    num_vars = len(metrics)
    angles = [n / float(num_vars) * 2 * pi for n in range(num_vars)]
    angles += angles[:1]
    
    # Plot each index
    for idx in normalized.index:
        values = normalized.loc[idx].tolist()
        values += values[:1]
        
        ax.plot(angles, values, 'o-', linewidth=2, label=idx, 
               color=colors[idx], markersize=8)
        ax.fill(angles, values, alpha=0.15, color=colors[idx])
    
    # Fix axis
    ax.set_xticks(angles[:-1])
    ax.set_xticklabels(metrics, fontsize=12, fontweight='bold')
    ax.set_ylim(0, 100)
    ax.set_yticks([20, 40, 60, 80, 100])
    ax.set_yticklabels(['20', '40', '60', '80', '100'], fontsize=10)
    ax.grid(True, linestyle='--', alpha=0.5)
    
    plt.legend(loc='upper right', bbox_to_anchor=(1.3, 1.1), fontsize=12, framealpha=0.9)
    plt.title('Comprehensive Performance Comparison\n(Higher is Better)', 
             fontsize=15, fontweight='bold', pad=30)
    
    plt.tight_layout()
    plt.savefig(output_dir / "6_radar_chart.png", dpi=300, bbox_inches='tight')
    print("✓ Generated: 6_radar_chart.png")
    plt.close()

# =====================================================================
# Main execution
# =====================================================================
if __name__ == "__main__":
    print("\n" + "="*60)
    print("  Generating Performance Comparison Plots")
    print("="*60 + "\n")
    
    plot_index_size()
    plot_build_time()
    plot_query_time_by_workload()
    plot_avg_query_time_log()
    plot_query_percentiles()
    plot_radar_chart()
    
    print("\n" + "="*60)
    print("  ✓ All plots generated successfully!")
    print(f"  Output directory: {output_dir}")
    print("="*60 + "\n")
    
    print("Generated plots:")
    print("  1. 1_index_size_comparison.png    - Index memory footprint")
    print("  2. 2_build_time_comparison.png     - Index construction time")
    print("  3. 3_query_time_by_workload.png    - Query performance per workload")
    print("  4. 4_avg_query_time_log.png        - Overall query performance")
    print("  5. 5_query_percentiles.png         - P95/P99 latency")
    print("  6. 6_radar_chart.png               - Comprehensive comparison")
    print()

