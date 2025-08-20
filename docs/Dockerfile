FROM ubuntu:22.04
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install --yes build-essential \
    cmake \
    doxygen \
    git \
    sudo \
    python3 \
    python3-pip \
    gdb \
    libgmp-dev

RUN pip install numpy \
    matplotlib \
    graphviz

ENTRYPOINT ["echo", "Hello from pqc-engineering-ssec-23 Docker container!"]