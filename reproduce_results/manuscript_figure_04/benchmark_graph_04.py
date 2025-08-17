import os

import numpy as np
import matplotlib.pyplot as plt
import re

barWidth = 0.25
fig = plt.subplots(figsize =(12, 8))

pattern = re.compile("(32m)([0-9]+)")

our_solution = []
qfesta = []


# Check for p381
matching_values = []
for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-04/benchmarks_ssec-p381-output.txt')):
    for match in re.finditer(pattern, line):
        matching_values.append(int(match.group(2)))
our_solution.append(matching_values[1])

# Check for p398
matching_values = []
for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-04/benchmarks_ssec-p398-output.txt')):
    for match in re.finditer(pattern, line):
        matching_values.append(int(match.group(2)))
qfesta.append(matching_values[1])

# Check for p575
matching_values = []
for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-04/benchmarks_ssec-p575-output.txt')):
    for match in re.finditer(pattern, line):
        matching_values.append(int(match.group(2)))
our_solution.append(matching_values[1])

# Check for p592
matching_values = []
for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-04/benchmarks_ssec-p592-output.txt')):
    for match in re.finditer(pattern, line):
        matching_values.append(int(match.group(2)))
qfesta.append(matching_values[1])

# Check for p765
matching_values = []
for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-04/benchmarks_ssec-p765-output.txt')):
    for match in re.finditer(pattern, line):
        matching_values.append(int(match.group(2)))
our_solution.append(matching_values[1])

# Check for p783
matching_values = []
for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-04/benchmarks_ssec-p783-output.txt')):
    for match in re.finditer(pattern, line):
        matching_values.append(int(match.group(2)))
qfesta.append(matching_values[1])


IT = our_solution
ECE = qfesta

br1 = np.arange(len(IT))
br2 = [x + barWidth for x in br1]

plt.bar(br1, IT, color ='r', width = barWidth, edgecolor ='grey', label ='Our Solution')
plt.bar(br2, ECE, color ='b', width = barWidth, edgecolor ='grey', label ='QFESTA')

plt.xlabel('Primes', fontweight ='bold', fontsize = 15)
plt.ylabel('CPU Cycles', fontweight ='bold', fontsize = 15)
plt.xticks([r + barWidth for r in range(len(IT))],
           ['128-bits: p381 vs p398', '192-bits: p575 vs p592', '256-bits: p765 vs. p783'])

plt.title('Benchmarks for the 3-isogenies walks: Our solution vs. QFESTA')

plt.legend()

graph_save_dir = "../generated_figures"
graph_04_save_dir = "../generated_figures/figure_04_output"
if not os.path.exists(graph_save_dir):
    os.makedirs(graph_save_dir)
if not os.path.exists(graph_04_save_dir):
    os.makedirs(graph_04_save_dir)

plt.savefig(f'../generated_figures/figure_04_output/figure_04_graph.pdf', format="pdf", bbox_inches="tight")


plt.show()