FROM ubuntu:18.04
RUN apt update && apt install -y ca-certificates

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends git curl wget ninja-build build-essential gdb python3-tk python3-pip python3-setuptools python3-dev python3-venv

RUN DEBIAN_FRONTEND=noninteractive apt-get -y install --no-install-recommends software-properties-common

RUN wget -O cmake.sh https://github.com/Kitware/CMake/releases/download/v3.20.5/cmake-3.20.5-linux-x86_64.sh \
    && sh cmake.sh --prefix=/usr/local/ --exclude-subdir && rm -rf cmake.sh

RUN DEBIAN_FRONTEND=noninteractive apt-get -y install --no-install-recommends clang-format

RUN git clone --depth 1 -b poco-serial-1.11.0-release https://github.com/vtpl1/poco.git \
    && cd poco && mkdir cmake-build && cd cmake-build && cmake -DBUILD_SHARED_LIBS=NO .. && cmake --build . --config Release --target install && cmake --build . --config Debug --target install \
    && cd ../.. && rm -rf poco

RUN git clone --depth 1 -b 1.1.5 https://github.com/nanomsg/nanomsg.git \
    && cd nanomsg && mkdir cmake-build && cd cmake-build && cmake -DNN_STATIC_LIB=ON .. && cmake --build . --config Release --target install && cmake --build . --config Debug --target install \
    && cd ../.. && rm -rf nanomsg && ldconfig

RUN git clone --depth 1 --origin "v1.9.2" "https://github.com/gabime/spdlog.git" \
    && cd spdlog && mkdir cmake-build && cd cmake-build && cmake .. && cmake --build . --config Release --target install && cmake --build . --config Debug --target install \
    && cd ../.. && rm -rf spdlog

RUN git clone --depth 1 --origin "lts_2021_03_24" "https://github.com/abseil/abseil-cpp.git" \
    && cd abseil-cpp && mkdir cmake-build && cd cmake-build && cmake -DABSL_ENABLE_INSTALL=ON .. && cmake --build . --config Release --target install && cmake --build . --config Debug --target install \
    && cd ../.. && rm -rf abseil-cpp

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME

RUN groupmod --gid $USER_GID $USERNAME \
    && usermod --uid $USER_UID --gid $USER_GID $USERNAME \
    && chown -R $USER_UID:$USER_GID /home/$USERNAME

# EXPOSE 9798