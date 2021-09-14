FROM centos:7

# run first to improve caching (other things update more often than SDL2)
WORKDIR /tmp
RUN curl -O https://www.libsdl.org/release/SDL2-2.0.16.tar.gz
RUN tar -xf SDL2-2.0.16.tar.gz
RUN mkdir SDL2-2.0.16/build
RUN curl -O https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz
RUN tar -xf SDL2_mixer-2.0.4.tar.gz
RUN mkdir SDL2_mixer-2.0.4/build

# add EPEL (for SDL2)
RUN yum -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm

# install dependencies
RUN yum -y install \
    # used below
    yum-utils \
    # SDL2_mixer dependencies
    libogg-devel libvorbis-devel \
    # VVVVVV dependencies
    gcc-c++ cmake make

RUN yum-builddep -y SDL2

RUN yum clean all

WORKDIR /tmp/SDL2-2.0.16/build
RUN ../configure
RUN make -j $(nproc)
RUN make install

WORKDIR /tmp/SDL2_mixer-2.0.4/build
RUN ../configure
RUN make -j $(nproc)
RUN make install

WORKDIR /tmp
RUN rm -rf SDL2-2.0.16.tar.gz SDL2-2.0.16/ SDL2_mixer-2.0.4.tar.gz SDL2_mixer-2.0.4/

WORKDIR /
