FROM ubuntu:24.04

ARG VERSION
ENV GCC_ARM_PATH        /usr/local
ENV GCC_VERSION         10.2.1
ENV VERSION             10-2020-q4-major

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
https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2?revision=ca0cbf9c-9de2-491c-ac48-898b5bbc0443&hash=B47BBB3CB50E721BC11083961C4DF5CA

RUN wget -c ${LINK_10_2020_q4_major} -O -| tar -xj -C ${GCC_ARM_PATH}

ENV PATH "${GCC_ARM_PATH}/gcc-arm-none-eabi-${VERSION}/bin:$PATH"

WORKDIR /usr/src/app
