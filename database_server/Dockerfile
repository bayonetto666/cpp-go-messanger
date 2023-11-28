FROM mongodb/mongo-cxx-driver:3.9.0-redhat-ubi-9.3

ENV MY_INSTALL_DIR /root/.local

RUN mkdir -p $MY_INSTALL_DIR

ENV PATH="$MY_INSTALL_DIR/bin:$PATH"

RUN microdnf install -y cmake make wget gcc-c++ autoconf libtool pkgconfig git

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

RUN microdnf install -y openssl openssl-devel

RUN git clone --depth 1 --shallow-submodules https://github.com/nlohmann/json

WORKDIR /

COPY ["database_server/CMakeLists.txt", "database_server/DatabaseHandler.cpp", "database_server/DatabaseHandler.hpp", "database_server/DatabaseService.hpp", "database_server/main.cpp", "/"]
COPY ../common /common

RUN cmake ..
RUN make

CMD [ "./database_server" ]