#!/usr/bin/env python3
"""
Generate plots for Aperture paper experimental results
"""

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# Read the CSV data
df = pd.read_csv('depth_analysis.csv')

# Create the plot
fig, ax = plt.subplots(figsize=(8, 6))

# Plot each workload type
ax.plot(df['Depth'], df['Analytics'], marker='o', label='Analytics Query', linewidth=2)
ax.plot(df['Depth'], df['ML'], marker='s', label='ML Training', linewidth=2)
ax.plot(df['Depth'], df['Financial'], marker='^', label='Financial Model', linewidth=2)

# Formatting
ax.set_xlabel('Aperture Depth', fontsize=12)
ax.set_ylabel('Computation Completed (%)', fontsize=12)
ax.set_title('Computation Completed Before Aperture Barriers', fontsize=14, fontweight='bold')
ax.grid(True, alpha=0.3)
ax.legend(loc='lower right')
ax.set_xlim(0.5, 8.5)
ax.set_ylim(0, 105)

# Add percentage signs to y-axis
ax.yaxis.set_major_formatter(plt.FuncFormatter(lambda x, p: f'{int(x)}%'))

# Save the figure
plt.tight_layout()
plt.savefig('paper/depth_analysis.pdf', dpi=300)
plt.savefig('paper/depth_analysis.png', dpi=150)
print("Saved plots to paper/depth_analysis.pdf and paper/depth_analysis.png")

# Also create a bar chart for Table 1 data
fig2, ax2 = plt.subplots(figsize=(10, 6))

operations = ['Arithmetic', 'Nested', 'Conditional', 'Let Binding', 'Function Call', 'Deep Nesting']
times = [1.41, 2.00, 1.83, 2.15, 2.58, 2.71]
errors = [0.098, 0.134, 0.128, 0.071, 0.478, 0.418]

x = np.arange(len(operations))
bars = ax2.bar(x, times, yerr=errors, capsize=5, color='steelblue', alpha=0.8)

ax2.set_xlabel('Operation Type', fontsize=12)
ax2.set_ylabel('Time (ms for 1000 iterations)', fontsize=12)
ax2.set_title('Aperture Operation Overhead', fontsize=14, fontweight='bold')
ax2.set_xticks(x)
ax2.set_xticklabels(operations, rotation=45, ha='right')
ax2.grid(True, axis='y', alpha=0.3)

# Add value labels on bars
for bar, val in zip(bars, times):
    height = bar.get_height()
    ax2.text(bar.get_x() + bar.get_width()/2., height,
             f'{val:.2f}', ha='center', va='bottom')

plt.tight_layout()
plt.savefig('paper/operation_overhead.pdf', dpi=300)
plt.savefig('paper/operation_overhead.png', dpi=150)
print("Saved operation overhead plots")

# Create a plot for multi-party protocol scalability
fig3, (ax3a, ax3b) = plt.subplots(1, 2, figsize=(12, 5))

parties = [3, 5, 10, 20, 50]
times = [15, 31, 65, 309, 1260]
bandwidth = [3.6, 10.0, 40.0, 160.0, 1000.0]

# Time scaling plot
ax3a.plot(parties, times, marker='o', linewidth=2, color='darkgreen')
ax3a.set_xlabel('Number of Parties', fontsize=12)
ax3a.set_ylabel('Total Time (ms)', fontsize=12)
ax3a.set_title('Protocol Time Scaling', fontsize=12, fontweight='bold')
ax3a.grid(True, alpha=0.3)
ax3a.set_xscale('log')
ax3a.set_yscale('log')

# Bandwidth scaling plot
ax3b.plot(parties, bandwidth, marker='s', linewidth=2, color='darkred')
ax3b.set_xlabel('Number of Parties', fontsize=12)
ax3b.set_ylabel('Bandwidth (KB)', fontsize=12)
ax3b.set_title('Protocol Bandwidth Scaling', fontsize=12, fontweight='bold')
ax3b.grid(True, alpha=0.3)
ax3b.set_xscale('log')
ax3b.set_yscale('log')

plt.tight_layout()
plt.savefig('paper/multiparty_scaling.pdf', dpi=300)
plt.savefig('paper/multiparty_scaling.png', dpi=150)
print("Saved multi-party protocol scaling plots")

# Create accuracy heatmap for LLM integration
fig4, ax4 = plt.subplots(figsize=(8, 6))

domains = ['Tax Rates', 'Physical Constants', 'Market Data', 'Legal Terms', 'Medical Codes']
accuracy = [91.4, 99.0, 76.2, 79.1, 87.0]
confidence = [0.98, 0.90, 0.73, 0.72, 0.80]

x = np.arange(len(domains))
width = 0.35

bars1 = ax4.bar(x - width/2, accuracy, width, label='Accuracy (%)', color='teal', alpha=0.8)
bars2 = ax4.bar(x + width/2, [c*100 for c in confidence], width, label='Confidence (%)', color='coral', alpha=0.8)

ax4.set_xlabel('Domain', fontsize=12)
ax4.set_ylabel('Percentage', fontsize=12)
ax4.set_title('LLM Inference Performance by Domain', fontsize=14, fontweight='bold')
ax4.set_xticks(x)
ax4.set_xticklabels(domains, rotation=45, ha='right')
ax4.legend()
ax4.grid(True, axis='y', alpha=0.3)

plt.tight_layout()
plt.savefig('paper/llm_accuracy.pdf', dpi=300)
plt.savefig('paper/llm_accuracy.png', dpi=150)
print("Saved LLM accuracy plots")

print("\nAll plots generated successfully!")
print("Files created in paper/ directory:")
print("  - depth_analysis.pdf/png")
print("  - operation_overhead.pdf/png")
print("  - multiparty_scaling.pdf/png")
print("  - llm_accuracy.pdf/png")