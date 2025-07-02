FROM ubuntu:24.04

ARG VERSION
ENV GCC_ARM_PATH        /usr/local
ENV GCC_VERSION         10.3.1
ENV VERSION             10.3-2021.10

# From https://github.com/hoo2/gcc-arm-none-eabi-docker/blob/master/Dockerfile
RUN apt-get update                           && \
    apt-get upgrade -y                       && \
    apt-get install --no-install-recommends -y  \
        build-essential                         \
        git                                     \
        bzip2                                   \
        wget                                    \
        ca-certificates                      && \
    apt-get clean                            && \
    rm -rf /var/lib/apt/lists/*              && \
    mkdir -p ${GCC_ARM_PATH}

ENV LINK_10_2020_q4_major \
https://developer.arm.com/-/media/Files/downloads/gnu-rm/${VERSION}/gcc-arm-none-eabi-${VERSION}-x86_64-linux.tar.bz2?rev=78196d3461ba4c9089a67b5f33edf82a&hash=5631ACEF1F8F237389F14B41566964EC
RUN wget -c ${LINK_10_2020_q4_major} -O -| tar -xj -C ${GCC_ARM_PATH}

ENV PATH "${GCC_ARM_PATH}/gcc-arm-none-eabi-${VERSION}/bin:$PATH"

WORKDIR /usr/src/app
