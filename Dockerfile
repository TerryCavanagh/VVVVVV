FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install -y ninja-build cmake libsdl2-dev libsdl2-mixer-dev && \
    rm -rf /var/lib/apt/lists/*
