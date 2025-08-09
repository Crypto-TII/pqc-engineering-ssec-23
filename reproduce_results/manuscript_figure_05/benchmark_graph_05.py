import os

import numpy as np
import matplotlib.pyplot as plt
import re

barWidth = 0.25
fig = plt.subplots(figsize =(12, 8))

pattern = re.compile(f"(total action experiments\s?)([0-9]+\s?Mcyc\s?)([0-9.]+)")

our_solution = []
qfesta = []


# Check for p381
matching_values_keygen_194 = []
matching_values_act_194 = []

matching_values_keygen_205 = []
matching_values_act_205 = []

matching_values_keygen_226 = []
matching_values_act_226 = []

x_dim = [0, 1, 2, 3, 4, 5]

for opt_rand in x_dim:
    for i, line in enumerate(open(f'statistics_output/analyzed_bench_keygen_194_opt_rad_{opt_rand}.txt')):
        for match in re.finditer(pattern, line):
            matching_values_keygen_194.append(float(match.group(3)))

    for i, line in enumerate(open(f'statistics_output/analyzed_bench_act_194_opt_rad_{opt_rand}.txt')):
        for match in re.finditer(pattern, line):
            matching_values_act_194.append(float(match.group(3)))

    for i, line in enumerate(open(f'statistics_output/analyzed_bench_keygen_205_opt_rad_{opt_rand}.txt')):
        for match in re.finditer(pattern, line):
            matching_values_keygen_205.append(float(match.group(3)))

    for i, line in enumerate(open(f'statistics_output/analyzed_bench_act_205_opt_rad_{opt_rand}.txt')):
        for match in re.finditer(pattern, line):
            matching_values_act_205.append(float(match.group(3)))

    for i, line in enumerate(open(f'statistics_output/analyzed_bench_keygen_226_opt_rad_{opt_rand}.txt')):
        for match in re.finditer(pattern, line):
            matching_values_keygen_226.append(float(match.group(3)))

    for i, line in enumerate(open(f'statistics_output/analyzed_bench_act_226_opt_rad_{opt_rand}.txt')):
        for match in re.finditer(pattern, line):
            matching_values_act_226.append(float(match.group(3)))

# for i in [0, 1, 2, 3, 4]:
#     print(f'{matching_values_keygen_194[i]}')
#
# for i in [0, 1, 2, 3, 4]:
#     print(f'{matching_values_act_194[i]}')

plt.figure(1)
plt.plot(x_dim, matching_values_keygen_194, color = 'g', marker='o', label='dCTIDH-p194 keygen')
plt.plot(x_dim, matching_values_act_194, color = 'y', marker='o', label='dCTIDH-p194 derive')

plt.plot(x_dim, matching_values_keygen_205, color = 'r', marker='*', label='dCTIDH-p205 keygen')
plt.plot(x_dim, matching_values_act_205, color = 'orange', marker='*', label='dCTIDH-p205 derive')

plt.plot(x_dim, matching_values_keygen_226, color = 'b', marker='+', label='dCTIDH-p226 keygen')
plt.plot(x_dim, matching_values_act_226, color = 'c', marker='+', label='dCTIDH-p226 derive')

plt.xlabel('m', fontweight ='bold', fontsize = 15)
plt.ylabel('CPU Cycles', fontweight ='bold', fontsize = 15)
plt.xticks(x_dim)

plt.title('Benchmarks for state-of-the-art dCTIDH vs. dCTIDH modified using our proposal (Keygen + Derive)')
plt.grid(True)
plt.legend()

graph_save_dir = "../generated_figures"
graph_05_save_dir = "../generated_figures/figure_05_output"
if not os.path.exists(graph_save_dir):
    os.makedirs(graph_save_dir)
if not os.path.exists(graph_05_save_dir):
    os.makedirs(graph_05_save_dir)

plt.savefig(f'../generated_figures/figure_05_output/figure_05_01_graph.pdf', format="pdf", bbox_inches="tight")


plt.figure(2)
plt.plot(x_dim, matching_values_act_194, color = 'y', marker='o', label='dCTIDH-p194 derive')
plt.plot(x_dim, matching_values_act_205, color = 'orange', marker='*', label='dCTIDH-p205 derive')
plt.plot(x_dim, matching_values_act_226, color = 'c', marker='+', label='dCTIDH-p226 derive')

plt.xlabel('m', fontweight ='bold', fontsize = 15)
plt.ylabel('CPU Cycles', fontweight ='bold', fontsize = 15)
plt.xticks(x_dim)

plt.title('Benchmarks for state-of-the-art dCTIDH vs. dCTIDH modified using our proposal  (Derive Only)')
plt.grid(True)
plt.legend()

plt.savefig(f'../generated_figures/figure_05_output/figure_05_02_graph.pdf', format="pdf", bbox_inches="tight")



plt.show()