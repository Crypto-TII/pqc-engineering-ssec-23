# Let us walk on the 3-isogeny graph: efficient, fast, and simple

[![Build and testing multiple primes' targets.](https://github.com/Crypto-TII/pqc-engineering-ssec-23/actions/workflows/cmake-multi-platform.yml/badge.svg?branch=main)](https://github.com/Crypto-TII/pqc-engineering-ssec-23/actions/workflows/cmake-multi-platform.yml)

Accompanying repository to the manuscript titled [“Let us walk on the 3-isogeny graph: efficient, fast, and simple”](https://eprint.iacr.org/2025/691).

## Table of contents
1. [Introduction](#section_01)  
2. [Setup Process](#section_02)
   1. [Build](#section_02_01)  
   2. [Testing](#section_02_02)
3. [Benchmarking](#section_03)
4. [Reproducing the Manuscript Results](#section_04)
   1. [Figure 3: Benchmarks for the 2-isogenies vs. 3-isogenies walks](#section_04_01)
   2. [Figure 4: Benchmarks for the 3-isogenies walks (Our solution vs. QFESTA)](#section_04_02)
   3. [Figure 5 (a) and Figure 5 (b): Benchmarks for state-of-the-art dCTIDH vs. dCTIDH modified using our proposal.](#section_04_03)
5. [Source-Code Technical Documentation: Doxygen](#section_05)
6. [Integrated CI/CD: Build, Test, Benchmarking, and Reporting](#section_06)
7. [How to download our public Docker container?](#section_07)
8. [Additional Resources' Build Process](#section_08)
9. [Conclusions, Acknowledgements and Authors](#section_09)

<a name="section_01"></a>
## 1. Introduction

Our paper reached several important results:
- This work centers on improving HASH functions (CGL Function), KEMs (QFESTA) and NIKEs (CTIDH). 
- Our results help to propose friendly parameters for QFESTA, along with the first efficient implementation in C of the radical 3-isogenies. 
- Our results speedup the dCTIDH-2048  by a 4x factor, without any considerable change in the parameter sets and allowing a straightforward integration (just replacing small isogenies of degree 3,5,7,11 and 13 by the aforementioned radical 3-isogenies).

A video summarizing our ideas and contribution (in a general-reader level) is shown below:

<p align="center">
<a href="https://www.youtube.com/watch?v=BjedMooSV30&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=1" target="_blank">
 <img src="http://img.youtube.com/vi/BjedMooSV30/0.jpg" alt="Watch the video" width="300" border="10" />
</a>
</p>

The YouTube link of our video is shown here: [Let us walk on the 3-isogeny graph: efficient, fast, and simple](https://www.youtube.com/watch?v=BjedMooSV30&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=1).

A general tree description of the source code of our project is shown below.

```
📁 pqc-engineering-ssec-23
├───📁 .github
├───📁 c-code
├───📁 dCTIDH
├───📁 docs
├───📁 gifs
├───📁 high-level-scripts
├───📁 obtained_statistics_examples
├───📁 reproduce_results
└───📄 README.md
```
Our source-code technical documentation is shown here: [Let us walk on the 3-isogeny graph: Technical Documentation](https://crypto-tii.github.io/pqc-engineering-ssec-23/). An example of interactive and clickable functions-dependency graphs [is shown here.](https://crypto-tii.github.io/pqc-engineering-ssec-23/html/df/d89/c-code_2src_2isogeny__walks__3__fp_8c.html)

In the following sections, we will cover in detail:
1. How to build, test, and benchmark,
2. How to replicate the results reported in the manuscript, 
3. How to generate the source code technical documentation using Doxygen,
4. A real-life production CI/CD pipeline integration, showing how to download our manuscript's results as public artifacts, and
5. Detailed instructions on how to download and mount our Docker container.

A detailed (full) video tour of our artifact is shown here: [Let us walk on the 3-isogeny graph: A (full) guided tour of our GitHub Artifact](https://www.youtube.com/watch?v=hLk_B5NpKRA&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=10).

For convenience, a list of all the videos of each individual section is shown below.

<table align="center"> 
<tr style="width: 100%"> 
<td align="center" style="width: 33%"> 
<a href="https://www.youtube.com/watch?v=xH9wxWVgauo&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=2"> 
<img src="https://img.youtube.com/vi/xH9wxWVgauo/0.jpg">
<br> <b>1. Overview and Introduction to our artifact</b> </a> 
</td> 
<td align="center" style="width: 33%"> 
<a href="https://www.youtube.com/watch?v=paQdkb1SVGs&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=3"> 
<img src="https://img.youtube.com/vi/paQdkb1SVGs/0.jpg">
<br> <b>2. How to Install, Build and Test our source code?</b> </a> 
</td> 
<td align="center" style="width: 33%"> 
<a href="https://www.youtube.com/watch?v=wCXyVPfD3lM&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=4"> 
<img src="https://img.youtube.com/vi/wCXyVPfD3lM/0.jpg">
<br> <b>3. How to benchmark our project: The Dos and Dont's</b> </a> 
</td> 
</tr>
<tr style="width: 100%"> 
<td align="center" style="width: 33%"> 
<a href="https://www.youtube.com/watch?v=kOgkQY2_Tr8&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=5"> 
<img src="https://img.youtube.com/vi/kOgkQY2_Tr8/0.jpg">
<br> <b>4. How to Replicate our Manuscript's Results?</b> </a> 
</td> 
<td align="center" style="width: 33%"> 
<a href="https://www.youtube.com/watch?v=PdysZFECqJk&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=6"> 
<img src="https://img.youtube.com/vi/PdysZFECqJk/0.jpg">
<br> <b>5. How to Generate the Source Code Technical Documentation?</b> </a> 
</td> 
<td align="center" style="width: 33%"> 
<a href="https://www.youtube.com/watch?v=Z8lkkB7D3BE&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=7"> 
<img src="https://img.youtube.com/vi/Z8lkkB7D3BE/0.jpg">
<br> <b>6. CI Pipeline in action: Real-World Software Development Demo</b> </a> 
</td> 
</tr>
<tr style="width: 100%"> 
<td align="center" style="width: 33%"> 
<a href="https://www.youtube.com/watch?v=TQrU4osynUg&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=8"> 
<img src="https://img.youtube.com/vi/TQrU4osynUg/0.jpg">
<br> <b>7. How to Download our Docker Container?</b> </a> 
</td> 
<td align="center" style="width: 33%"> 
<a href="https://www.youtube.com/watch?v=JHZuAFgPtuY&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=9"> 
<img src="https://img.youtube.com/vi/JHZuAFgPtuY/0.jpg">
<br> <b>8. Conclusions</b> </a> 
</td>
<td align="center" style="width: 33%"> 
<a href="https://www.youtube.com/watch?v=hLk_B5NpKRA&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=10"> 
<img src="https://img.youtube.com/vi/hLk_B5NpKRA/0.jpg">
<br> <b>(Full) guided tour of our artifact</b> </a> 
</td> 
</tr>
</table>

A slide-style presentation with a summary of the technical steps presented in this repository is also available here: [Let us walk on the 3-isogeny graph: Step-by-step Artifact Walkthrough](docs/artifact_walkthrough.pdf).

<a name="section_02"></a>
## 2. Setup Process

In this section we present a setup process that can be run in any Linux terminal. In case a specialized IDE like CLion is desired, please refer to [Let us walk on the 3-isogeny graph: CLion Setup](README_Clion_IDE.md).

<a name="section_02_01"></a>
### 2.1. Build

#### System requirements

Our (physical) testbed consists of machine with a 12th Gen. Intel(R) Core(TM) i9-12900H CPU and 32 Gb of RAM, running Ubuntu 20.04.6 LTS (64 bits), but any Linux environment running in an Intel CPU is enough.

Our project works in any out-of-the-box Linux-based environment with some basic software requirements:
- Cmake
- Python3 (numpy and matplotlib)

To check if your system counts with the required software, simply run 
```shell
cmake --version
python3 --version
pip list | grep numpy
pip list | grep matplotlib
```
If all the requirements are met, the terminal should return installed versions like the ones below.
![](gifs/01-cmake-profile/numpy.png)

To build our project, in the root directory `pqc-engineering-ssec-23`, simply run
```shell
cd c-code
cmake -DCMAKE_BUILD_TYPE=Release -B cmake-build-release
cd cmake-build-release
make -j
```
This will create the `cmake-build-release` folder with all the tests for all the supported primes: `p254`, `p255`, `p381`, `p383`, `p398`, `p511`, `p575`, `p592`, `p765`, and `p783`. A list of the generated tests is shown below.

![](gifs/03-examples/01.png)

A demo of the whole process of setup and build process is shown below.

![](gifs/03-examples/build_release.gif)


<a name="section_02_02"></a>
### 2.2. Testing

In this section, we show how to perform the **testing** of our source code. For a detailed explanation of each testing mode, please refer to our additional documentation: [Let us walk on the 3-isogeny graph: (Detailed) Build, Test and Benchmarking Framework Documentation](c-code/README.md).

After building as shown in the previous section, inside the `c-code/cmake-build-release` folder, locate all the possible tests with 
```shell
ls ./tests/
```
To execute any particular test, simply select one of the following 
```shell
./tests/tests-ssec-p254
./tests/tests-ssec-p255
./tests/tests-ssec-p381
./tests/tests-ssec-p383
./tests/tests-ssec-p398
./tests/tests-ssec-p511
./tests/tests-ssec-p575
./tests/tests-ssec-p592
./tests/tests-ssec-p765
./tests/tests-ssec-p783
```
For example, the execution of `./tests/tests-ssec-p254` is shown below.

![](gifs/03-examples/testing.gif)

To run ALL the tests in verbose mode, simply run
```
ctest -V -R test
```

A demo of all the tests running in verbose mode is shown below.

![](gifs/03-examples/testing_verbose.gif)

<a name="section_03"></a>
## 3. Benchmarking

In this section, we show how to perform the **benchmarking** of our source code. 

As supplementary material: 
1. A detailed walkthrough of the steps in this section is available in our YouTube video: [Modulo 3: How to benchmark our project? The Dos and Dont's](https://www.youtube.com/watch?v=wCXyVPfD3lM&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=4). 
2. For an explanation of how to perform the benchmarks in a detailed mode (and more insights about the used CPU benchmarking method), please refer to our additional documentation: [Let us walk on the 3-isogeny graph: (Detailed) Build, Test and Benchmarking Framework Documentation](c-code/README.md).

For benchmarking, the correct commands must be used when doing the first cmake. Inside the root directory `pqc-engineering-ssec-23`, simply run

```bash
cd c-code
cmake -DCMAKE_BUILD_TYPE=Release -DBENCHMARKING=CYCLES -DARCHITECTURE=x8664 -B cmake-build-release-cycles-x8664
```
followed by
```bash
cd cmake-build-release-cycles-x8664
make -j
```

**NOTE:** Benchmarking does not work for
```bash
cmake -DCMAKE_BUILD_TYPE=Release -B cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Debug -B cmake-build-debug
```
In case you run the benchmarking in either one of these two build modes (without the `-DBENCHMARKING` and the `-DARCHITECTURE` flags), you will get the following error:

![](gifs/03-examples/benchmarks_error.png)

To execute any particular benchmarking, inside the `cmake-build-release-cycles-x8664` folder, simply select one of the following
```shell
benchmarks/benchmarks-ssec-p254
benchmarks/benchmarks-ssec-p255
benchmarks/benchmarks-ssec-p381
benchmarks/benchmarks-ssec-p383
benchmarks/benchmarks-ssec-p398
benchmarks/benchmarks-ssec-p511
benchmarks/benchmarks-ssec-p575
benchmarks/benchmarks-ssec-p592
benchmarks/benchmarks-ssec-p765
benchmarks/benchmarks-ssec-p783
```

A demo of successful benchmarkings is shown below.

![](gifs/03-examples/benchmarking.gif)

<a name="section_04"></a>
## 4. Reproducing the Manuscript Results

In our [manuscript](https://eprint.iacr.org/2025/691), several statistical figures are shown. In this section, we cover how to replicate the obtained graphs. 

As supplementary material, a detailed walkthrough of the steps in this section is available in our YouTube video: [Modulo 4: How to Replicate our Manuscript's Results?](https://www.youtube.com/watch?v=kOgkQY2_Tr8&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=5)

In order to reproduce some of the figures in the manuscript, we provide with easy-to-use scripts that wrap all the required executions of the benchmarking tests, and by using `numpy` and `matplotlib`, generate the manuscript graphs.

The related code to reproduce our results is shown in the tree below.

```
📁 pqc-engineering-ssec-23
├───📁 c-code
├───📁 dCTIDH
├───📁 docs
├───📁 gifs
├───📁 high-level-scripts
├───📁 obtained_statistics_examples
├───📁 reproduce_results
│   ├───📁 generated_figures               # <= AUTOMATICALLY GENERATED!
│   ├───📁 manuscript_figure_03
│   │   ├───📄 benchmark_graph_03.py
│   │   └───📄 generate_figure_03.sh       # <= NEED TO EXECUTE
│   ├───📁 manuscript_figure_04
│   │   ├───📄 benchmark_graph_04.py
│   │   └───📄 generate_figure_04.sh       # <= NEED TO EXECUTE
│   └───📁 manuscript_figure_05
│       ├───📁 dCTIDH_benchmarks_output    # <= AUTOMATICALLY GENERATED!
│       ├───📁 dCTIDH_builds               # <= AUTOMATICALLY GENERATED!
│       ├───📁 statistics_output           # <= AUTOMATICALLY GENERATED!
│       ├───📄 analyze_bench.py
│       ├───📄 benchmark_graph_05.py
│       └───📄 generate_figure_05.sh       # <= NEED TO EXECUTE
└───📄 README.md
```

<a name="section_04_01"></a>
### 4.1. Figure 3: Benchmarks for the 2-isogenies vs. 3-isogenies walks

Inside the `reproduce_results/manuscript_figure_03` folder, it is necessary to give execution permissions to the script, via
```shell
chmod +x generate_figure_03.sh
```
Then, just simply execute it
```shell
./generate_figure_03.sh
```
This will automatically build with the `-DBENCHMARKING=CYCLES -DARCHITECTURE=x8664` flags, and perform all the statistics. At the end, a bar graph is automatically generated.

A demo of how to obtain the manuscript's Figure 03 is shown below.

![](gifs/04-replicate-results/fig_03/fig_03.gif)

where the original Figure 3 presented in the manuscript is shown below.

![](gifs/04-replicate-results/fig_03/figure_03_original.png)

A PDF is generated with the generated graph and stored inside the `reproduced_results/generated_figures/figure_03_output` folder. This folder will be automatically generated once the `generate_figure_03.sh` script executes successfully.
```
📁 reproduce_results
└───📁 generated_figures                # <= AUTOMATICALLY GENERATED!
    └───📁 figure_03_output
        └───📄 figure_03_graph.pdf      # <= AUTOMATICALLY SAVED AFTER SUCCESSFUL EXECUTION!
```

<a name="section_04_02"></a>
### 4.2. Figure 4: Benchmarks for the 3-isogenies walks (Our solution vs. QFESTA)

Similar to the previous figure, inside the `reproduce_results/manuscript_figure_04` folder, it is necessary to give execution permissions to the script, via
```shell
chmod +x generate_figure_04.sh
```
Then, just simply execute it
```shell
./generate_figure_04.sh
```
This will automatically build with the `-DBENCHMARKING=CYCLES -DARCHITECTURE=x8664` flags, and perform all the statistics. At the end, a bar graph is automatically generated.

A demo of how to obtain the manuscript's Figure 03 is shown below.

![](gifs/04-replicate-results/fig_04/fig_04.gif)

where the original Figure 4 presented in the manuscript is shown below.

![](gifs/04-replicate-results/fig_04/figure_04_original.png)

A PDF is generated with the generated graph and stored inside the `reproduced_results/generated_figures/figure_04_output` folder. This folder will be automatically generated once the `generate_figure_04.sh` script executes successfully.
```
📁 reproduce_results
└───📁 generated_figures                # <= AUTOMATICALLY GENERATED!
    └───📁 figure_04_output
        └───📄 figure_04_graph.pdf      # <= AUTOMATICALLY SAVED AFTER SUCCESSFUL EXECUTION!
```

<a name="section_04_03"></a>
### 4.2. Figure 5 (a) and Figure 5 (b): Benchmarks for state-of-the-art dCTIDH vs. dCTIDH modified using our proposal.

Similar to the previous figures, inside the `reproduce_results/manuscript_figure_05` folder, it is necessary to give execution permissions to the script, via
```shell
chmod +x generate_figure_05.sh
```
Then, just simply execute it
```shell
./generate_figure_05.sh
```
This will automatically create three folders:
```
📁 manuscript_figure_05
├───📁 dCTIDH_benchmarks_output    # <= AUTOMATICALLY GENERATED!
├───📁 dCTIDH_builds               # <= AUTOMATICALLY GENERATED!
├───📁 statistics_output           # <= AUTOMATICALLY GENERATED!
├───📄 analyze_bench.py
├───📄 benchmark_graph_05.py
└───📄 generate_figure_05.sh       # <= NEED TO EXECUTE
```
The script will generate all the required folders to perform all the statistics. At the end, two line graphs are automatically generated. The first graph is the one associated to Figure 5 (a) in the manuscript, while the second graph is Figure 5 (b).

A demo of how to obtain the manuscript's Figure 05 is shown below.

![](gifs/04-replicate-results/fig_05/fig_05.gif)

where the original Figure 5 presented in the manuscript is shown below.

![](gifs/04-replicate-results/fig_05/figure_05_original.png)

Two PDF files are generated with the generated graphs and stored inside the `reproduced_results/generated_figures/figure_05_output` folder. This folder will be automatically generated once the `generate_figure_05.sh` script executes successfully.
```
📁 reproduce_results
└───📁 generated_figures                   # <= AUTOMATICALLY GENERATED!
    └───📁 figure_05_output
        ├───📄 figure_05_01_graph.pdf      # <= AUTOMATICALLY SAVED AFTER SUCCESSFUL EXECUTION!
        └───📄 figure_05_02_graph.pdf      # <= AUTOMATICALLY SAVED AFTER SUCCESSFUL EXECUTION!
```

<a name="section_05"></a>
## 5. Source-Code Technical Documentation: Doxygen

Our project supports automatic technical documentation generation via Doxygen. As supplementary material, a detailed walkthrough of the steps in this section is available in our YouTube video: [Modulo 5: How to Generate the Source Code Technical Documentation?](https://www.youtube.com/watch?v=PdysZFECqJk&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=6). 

To install Doxygen (and Graphviz) in case not installed in the system, simply run
```bash
sudo apt-get install doxygen graphviz
```

To generate the Doxygen documentation, inside the `docs` folder, simply run
```bash
doxygen Doxyfile
```
This will generate an HTML site with interactive diagrams, and plenty of technical documentation. A demo of the generated documentation is shown below.

![](gifs/05-doxygen/doxygen.gif)

A link to a public-hosted version of our source-code documentation is shown here: [Let us walk on the 3-isogeny graph: Technical Documentation](https://crypto-tii.github.io/pqc-engineering-ssec-23/)

<a name="section_06"></a>
## 6. Integrated CI/CD: Build, Test, Benchmarking, and Reporting

To prove that this project can be integrated in an industrial environment where Continuous Integration (CI) and Continuous Delivery (CD), we follow a classic CI/CD workflow of (1) Build, (2) Test, (3) Benchmark, and (4) Reporting. 

<p align="center">
  <img src="gifs/02-pipeline/ci-cd.gif" alt="Build, test and benchmark jobs." width="750">
</p>

As supplementary material, a detailed walkthrough of the information conveyed in this section is available in our YouTube video: [Modulo 6: CI Pipeline in action: Real-World Software Development Demo](https://www.youtube.com/watch?v=Z8lkkB7D3BE&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=7).

To provide CI/CD related capabilities, in our source code we provide a [cmake-multi-platform.yml](.github/workflows/cmake-multi-platform.yml) file that uses **Docker images** to build, test and benchmark our solution. This is done to prove that our code and contribution can be integrated in a pipeline and be delivered as a part of a cryptographic solution in an industrial scenario.

<p align="center">
  <img src="gifs/02-pipeline/pipeline-gif.gif" alt="Build, test and benchmark jobs." width="1000">
</p>

At the end of the Benchmark stage, the Reporting stage generates the three graphs presented in the manuscript (See [Section 4: Reproducing the Manuscript Results](#section_04)). All three generated graphs and all the benchmarking results (per prime) are uploaded as artifacts in the pipeline. In the figure below:
- The benchmark statistics are uploaded in the artifacts marked in **blue**, and
- The generated manuscript graphs are uploaded in the artifacts marked in **red**.
All the statistical data and all the graphs are uploaded as public artifacts to provide means to the reader to replicate our results. 

<p align="center">
  <img src="gifs/02-pipeline/artifacts_uploaded.png" alt="Uploaded artifacts." width="1000">
</p>

<a name="section_07"></a>
## 7. How to use our Docker container?

For the convenience of our readers and any scientist that would like to replicate our results, we provide (1) a publicly available Docker container, and (2) a Docker file. These two options offer our readers to download (or create) a system with all our software requirements. This provides a self-contained environment where our artifact runs out-of-the-box.

As supplementary material, a detailed walkthrough of the steps in this section is available in our YouTube video: [Modulo 7: How to Download our publicly available Docker Container?](https://www.youtube.com/watch?v=TQrU4osynUg&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=9).

Currently, natively only Intel CPUs are supported. To build, test, benchmark and replicate our results in an **Apple Silicon-based computer** (`M1`, `M2`, `M3`, `M4` CPus), in Docker Desktop, the following settings regarding Rosetta must be modified.


### 7.1 How to download our public Docker container?

To download our Docker container, simply execute the command below
```bash
docker pull tiicrc/github-selfhosted-runner-pqc:latest
```
and to verify that it was downloaded correctly, execute
```bash
docker images | grep pqc
```

### 7.2 How to locally build our Docker container?

In case it is desired to locally-build the container, the required Dockerfile can be found [here](docs/Dockerfile) (location: `docs/Dockerfile`). 

To build the local Docker container, simply execute
```bash
cd docs
sudo docker build -t pqc-engineering-ssec-23-local-docker .
```
and to verify that it was built correctly, execute
```bash
docker images | grep pqc
```
### 7.3 How to mount our Docker container?

To mount the Docker container, first locate your terminal at the artifact's root folder(`pqc-engineering-ssec-23`), and then:
1. In case the Docker container was **downloaded**, execute
```bash
docker run --rm -ti -v $PWD:/src -w /src tiicrc/github-selfhosted-runner-pqc:latest bash
```
2. In case the **local container was built**, then execute
```bash
docker run --rm -ti -v $PWD:/src -w /src pqc-engineering-ssec-23-local-docker:latest bash
```
After mounting, for either of both cases mentioned above, the terminal will change to
```bash
/src# <insert commands here>
```
At this point, all the steps presented in [Section 2: Setup Process](#section_02), all the benchmarking shown in [Section 3: Benchmarking](#section_03), all the experiments presented in [Section 4: Reproducing the Manuscript Results](#section_04), and all the steps to generate the technical documentation using Doxygen as shown in [Section 5: Source-Code Technical Documentation: Doxygen](#section_05) shall work without problems.

<a name="section_08"></a>
## 8. Additional Resources' Build Process

As mentioned before, for a detailed explanation of our testing and benchmarking frameworks (with insights of the CPU benchmarking approach), please refer to our additional documentation: [Let us walk on the 3-isogeny graph: (Detailed) Build, Test and Benchmarking Framework Documentation](c-code/README.md).

As part of our experiments, we used a modified version of `dCTIDH`. To build the modified `dCTIDH`, please refer to [Let us walk on the 3-isogeny graph: dCTIDH modified version](dCTIDH/README.md).

<a name="section_09"></a>
## 9. Conclusions, Acknowledgements and Authors

We sincerely thank the scientific community, our collaborators, and everyone who made our artifact possible — with special gratitude to our DevOps team for their invaluable work in setting up our self-hosted runner and infrastructure. Words alone cannot fully convey our gratitude to our collaborators. To express our conclusions and acknowledgments in a more vivid and engaging way, we invite to watch our video here: [Modulo 8: Conclusions](https://www.youtube.com/watch?v=JHZuAFgPtuY&list=PLFgwYy6Y-xWYCFruq66CFXXiWEWckEk6Q&index=9).

This project highlights the importance of clear, comprehensive documentation, building software that seamlessly integrates into continuous integration pipelines to meet industry standards, and leveraging Docker containers for reproducibility. 

We invite fellow researchers and practitioners to explore, test, and expand upon our work, helping it grow and evolve in new directions.

For further information, please feel free to contact any of the authors:
<p align="center">
    <a href="mailto:jesus.dominguez@tii.ae"><strong>Jesús-Javier Chi-Domínguez</strong></a>,
    <a href="mailto:eduardo.ochoa@tii.ae"><strong>Eduardo Ochoa-Jiménez</strong></a>,
    <a href="mailto:ricardo.pontaza@tii.ae"><strong>Ricardo-Neftalí Pontaza-Rodas</strong></a>.
</p>


