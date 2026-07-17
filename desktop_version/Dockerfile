FROM rockylinux:9

# run first to improve caching (other things update more often than SDL3)
WORKDIR /tmp
RUN curl -LJO https://github.com/libsdl-org/SDL/releases/download/release-3.4.12/SDL3-3.4.12.tar.gz
RUN tar -xf SDL3-3.4.12.tar.gz
RUN mkdir SDL3-3.4.12/build

# install dependencies
RUN dnf --assumeyes upgrade --refresh
RUN dnf --assumeyes group install development
RUN dnf --assumeyes install cmake
RUN dnf --assumeyes install libX11-devel libICE-devel libSM-devel \
    libXScrnSaver-devel libXext-devel libXft-devel libXi-devel \
    libXinerama-devel libXmu-devel libXp-devel libXpm-devel \
    libXrandr-devel libXrender-devel libXt-devel libXtst-devel \
    libXv-devel libXxf86dga-devel libxkbcommon-devel libXcursor-devel

WORKDIR /tmp/SDL3-3.4.12/build
RUN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
RUN cmake --build . --target install -- -j $(nproc)

WORKDIR /tmp
RUN rm -rf SDL3-3.4.12.tar.gz SDL3-3.4.12/

WORKDIR /
