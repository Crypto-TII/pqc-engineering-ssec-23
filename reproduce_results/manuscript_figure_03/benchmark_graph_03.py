import os

import numpy as np
import matplotlib.pyplot as plt
import re

barWidth = 0.25
fig = plt.subplots(figsize =(12, 8))

pattern = re.compile("(32m)([0-9]+)")

average_cglhash2 = []
average_cglhash3 = []

# 1. Check for p254
matching_values = []
for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-03/benchmarks_ssec-p254-output.txt')):
    for match in re.finditer(pattern, line):
        matching_values.append(int(match.group(2)))
average_cglhash2.append(matching_values[0])
average_cglhash3.append(matching_values[1])

# 2. Check for p252
matching_values = []
for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-03/benchmarks_ssec-p255-output.txt')):
    for match in re.finditer(pattern, line):
        matching_values.append(int(match.group(2)))
average_cglhash2.append(matching_values[0])
average_cglhash3.append(matching_values[1])

# # 3. Check for p381
# matching_values = []
# for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-03/benchmarks_ssec-p381-output.txt')):
#     for match in re.finditer(pattern, line):
#         matching_values.append(int(match.group(2)))
# average_cglhash2.append(matching_values[0])
# average_cglhash3.append(matching_values[1])

# # 4. Check for p383
# matching_values = []
# for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-03/benchmarks_ssec-p383-output.txt')):
#     for match in re.finditer(pattern, line):
#         matching_values.append(int(match.group(2)))
# average_cglhash2.append(matching_values[0])
# average_cglhash3.append(matching_values[1])
#
# # 5. Check for p398
# matching_values = []
# for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-03/benchmarks_ssec-p398-output.txt')):
#     for match in re.finditer(pattern, line):
#         matching_values.append(int(match.group(2)))
# average_cglhash2.append(matching_values[0])
# average_cglhash3.append(matching_values[1])
#
# # 6. Check for p511
# matching_values = []
# for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-03/benchmarks_ssec-p511-output.txt')):
#     for match in re.finditer(pattern, line):
#         matching_values.append(int(match.group(2)))
# average_cglhash2.append(matching_values[0])
# average_cglhash3.append(matching_values[1])
#
# # 7. Check for p575
# matching_values = []
# for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-03/benchmarks_ssec-p575-output.txt')):
#     for match in re.finditer(pattern, line):
#         matching_values.append(int(match.group(2)))
# average_cglhash2.append(matching_values[0])
# average_cglhash3.append(matching_values[1])
#
# # 8. Check for p592
# matching_values = []
# for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-03/benchmarks_ssec-p592-output.txt')):
#     for match in re.finditer(pattern, line):
#         matching_values.append(int(match.group(2)))
# average_cglhash2.append(matching_values[0])
# average_cglhash3.append(matching_values[1])
#
# # 9. Check for p765
# matching_values = []
# for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-03/benchmarks_ssec-p765-output.txt')):
#     for match in re.finditer(pattern, line):
#         matching_values.append(int(match.group(2)))
# average_cglhash2.append(matching_values[0])
# average_cglhash3.append(matching_values[1])
#
# # 10. Check for p783
# matching_values = []
# for i, line in enumerate(open('../../c-code/cmake-build-release-cycles-x8664-graph-03/benchmarks_ssec-p783-output.txt')):
#     for match in re.finditer(pattern, line):
#         matching_values.append(int(match.group(2)))
# average_cglhash2.append(matching_values[0])
# average_cglhash3.append(matching_values[1])


IT = average_cglhash2
ECE = average_cglhash3

br1 = np.arange(len(IT))
br2 = [x + barWidth for x in br1]

plt.bar(br1, IT, color ='r', width = barWidth, edgecolor ='grey', label ='2-isogenies')
plt.bar(br2, ECE, color ='b', width = barWidth, edgecolor ='grey', label ='3-isogenies')

plt.xlabel('Primes', fontweight ='bold', fontsize = 15)
plt.ylabel('CPU Cycles', fontweight ='bold', fontsize = 15)
plt.xticks([r + barWidth for r in range(len(IT))],
           ['p254', 'p255'])
# plt.xticks([r + barWidth for r in range(len(IT))],
#            ['p254', 'p255', 'p381', 'p383', 'p398', 'p511', 'p575', 'p592', 'p765', 'p783'])

plt.title('Benchmarks for the 2-isogenies vs. 3-isogenies walks')

plt.legend()

graph_save_dir = "../generated_figures"
graph_03_save_dir = "../generated_figures/figure_03_output"
if not os.path.exists(graph_save_dir):
    os.makedirs(graph_save_dir)
if not os.path.exists(graph_03_save_dir):
    os.makedirs(graph_03_save_dir)

plt.savefig(f'../generated_figures/figure_03_output/figure_03_graph.pdf', format="pdf", bbox_inches="tight")


plt.show()