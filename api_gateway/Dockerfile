FROM debian:stable 

ENV MY_INSTALL_DIR /root/.local

RUN mkdir -p $MY_INSTALL_DIR

ENV PATH="$MY_INSTALL_DIR/bin:$PATH"

RUN apt-get update && apt-get install -y cmake build-essential autoconf libtool pkg-config git make wget g++ 

RUN git clone --recurse-submodules -b v1.58.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc

WORKDIR /grpc

RUN mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      ../.. && \
    make -j 4 && \
    make install

WORKDIR /

RUN apt-get update
RUN apt install nlohmann-json3-dev
RUN apt install -y openssl
RUN apt install -y libssl-dev

RUN wget -O boost_1_82_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.82.0/boost_1_82_0.tar.gz/download

RUN tar -xvzf boost_1_82_0.tar.gz -C .


COPY ["api_gateway/CMakeLists.txt", "api_gateway/Server.cpp", "api_gateway/Server.hpp", "api_gateway/main.cpp", "api_gateway/websocket_proxy.cpp", "api_gateway/websocket_proxy.hpp", "/"]
COPY ../common /common

RUN cmake  ..
RUN make

CMD ["/.api_gateway"]
