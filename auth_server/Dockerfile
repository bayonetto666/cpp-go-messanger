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

RUN git clone https://github.com/Thalhammer/jwt-cpp

RUN git clone https://github.com/P-H-C/phc-winner-argon2
WORKDIR /phc-winner-argon2
RUN make


WORKDIR /
COPY ["auth_server/CMakeLists.txt", "auth_server/Auth.cpp", "auth_server/Auth.hpp", "auth_server/main.cpp", "auth_server/Hasher.cpp", "auth_server/Hasher.hpp", "auth_server/main.cpp", "auth_server/AuthService.hpp", "/"]
COPY ../common /common

RUN cmake  ..
RUN make

CMD ["./auth_server"]
