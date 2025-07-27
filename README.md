# Let us walk on the 3-isogeny graph: efficient, fast, and simple

Accompanying repository to the manuscript titled [“Let us walk on the 3-isogeny graph: efficient, fast, and simple”](https://eprint.iacr.org/2025/691).

## Table of contents
1. [Introduction](#section_01)  
2. [Setup Process](#section_02)  
   2.1. [IDE Setup: CLion](#section_02_01)  
   2.2. [General Terminal Setup](#section_02_02)
3. [Another paragraph](#paragraph2)


<a name="section_01"></a>
## 1. Introduction

Our paper reached several important results:
- This work centers on improving HASH functions (CGL Function), KEMs (QFESTA) and NIKEs (CTIDH). 
- Our results help to propose friendly parameters for QFESTA, along with the first efficient implementation in C of the radical 3-isogenies. 
- Our results speedup the dCTIDH-2048  by a 4x factor, without any considerable change in the parameter sets and allowing a straightforward integration (just replacing small isogenies of degree 3,5,7,11 and 13 by the aforementioned radical 3-isogenies).

A video summarizing our ideas and contribution (in a general-reader level) is shown below:

<a href="http://www.youtube.com/watch?feature=player_embedded&v=KhoViHztXpE" target="_blank">
 <img src="http://img.youtube.com/vi/KhoViHztXpE/mqdefault.jpg" alt="Watch the video" width="240" height="180" border="10" />
</a>

The YouTube link of our video is shown here: [Let us walk on the 3-isogeny graph: efficient, fast, and simple](https://www.youtube.com/watch?v=KhoViHztXpE).

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
In the following sections, we will cover in detail:
1. How to build, test, and benchmark,
2. How to replicate the results reported in the manuscript, 
3. How to generate the source code technical documentation using Doxygen, and
4. A real-life production CI/CD pipeline integration.

<a name="section_02"></a>
## 2. Setup Process

In this section we present a setup process that can be run in any Linux terminal. In case a specialized IDE like CLion is desired, please refer to [Let us walk on the 3-isogeny graph: CLion Setup](README_Clion_IDE.md).

<a name="section_02_01"></a>
### 2.1. Build

#### System requirements

Our project works out-of-the-box in any Linux-based environment with some basic software requirements:
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

An example of the whole process of setup and build is shown below.

![](gifs/03-examples/build_release.gif)


<a name="section_02_02"></a>
### 2.2. Testing

## Doxygen

To generate the Doxygen documentation of the project, please run

```bash
pqc-engineering-ssec-23/docs$ doxygen Doxyfile
```

## Integrated CI/CD testing and benchmarking

To prove that this project can be integrated in an industrial environment where Continuous Integration (CI) and Continuous Delivery (CD) is important, we provide a [cmake-multi-platform.yml](.github/workflows/cmake-multi-platform.yml) file that uses docker images to build, test and benchmark our solution. This is done to prove that our code and contribution can be integrated in a pipeline and be delivered as a part of a cryptographic solution in an industrial scenario.

<p align="center">
  <img src="gifs/02-pipeline/pipeline-gif.gif" alt="Build, test and benchmark jobs." width="500">
</p>

## Detailed build process

Please refer to `c-code/README.md` for a detailed description of the build process of the c code. To build the modified dCTIDH, please refer to `dCTIDH/README.md`.

Please refer to [c-code/README.md](https://github.com/Crypto-TII/pqc-engineering-ssec-23/tree/main/c-code/README.md) for a detailed description of the build process of the c code. To build the modified dCTIDH, please refer to [README.md](https://github.com/Crypto-TII/pqc-engineering-ssec-23/tree/main/dCTIDH/README.md).


## Authors

For further information, please feel free to contact any of the authors:
<p align="center">
    <a href="mailto:jesus.dominguez@tii.ae"><strong>Jesús-Javier Chi-Domínguez</strong></a>,
    <a href="mailto:eduardo.ochoa@tii.ae"><strong>Eduardo Ochoa-Jiménez</strong></a>,
    <a href="mailto:ricardo.pontaza@tii.ae"><strong>Ricardo-Neftalí Pontaza-Rodas</strong></a>.
</p>
