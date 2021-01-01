FROM centos:7

# run first to improve caching (other things update more often than SDL2)
WORKDIR /tmp
RUN curl -O https://www.libsdl.org/release/SDL2-2.0.14.tar.gz
RUN tar -xf SDL2-2.0.14.tar.gz
RUN mkdir SDL2-2.0.14/build
RUN curl -O https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz
RUN tar -xf SDL2_mixer-2.0.4.tar.gz
RUN mkdir SDL2_mixer-2.0.4/build

RUN yum -y install \
    # SDL2 dependencies, adapted from https://hg.libsdl.org/SDL/file/default/docs/README-linux.md
    cmake make autoconf automake \
    libtool alsa-lib-devel pulseaudio-libs-devel libX11-devel libXext-devel \
    libXrandr-devel libXcursor-devel libXi-devel libXinerama-devel libXxf86vm-devel \
    mesa-libGL-devel dbus-devel libudev-devel \
    ibus-devel libsamplerate-devel \
    wayland-devel libxkbcommon-devel \
    # SDL2_mixer dependencies
    libogg-devel libvorbis-devel \
    # VVVVVV dependencies
    gcc-c++ && \
    yum clean all

WORKDIR /tmp/SDL2-2.0.14/build
RUN ../configure
RUN make -j $(nproc)
RUN make install

WORKDIR /tmp/SDL2_mixer-2.0.4/build
RUN ../configure
RUN make -j $(nproc)
RUN make install

WORKDIR /tmp
RUN rm -rf SDL2-2.0.14.tar.gz SDL2-2.0.14/ SDL2_mixer-2.0.4.tar.gz SDL2_mixer-2.0.4/

WORKDIR /
