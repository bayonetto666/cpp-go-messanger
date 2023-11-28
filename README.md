
# cpp-go-messanger

Cpp-go-messenger is a straightforward example of a real-time messenger implemented in C++ and Go,
utilizing a microservices architecture. 
The project also includes a basic Go client with a graphical user interface (using Fyne).
![scheme of microservises](https://github.com/bayonetto666/cpp-go-messanger/blob/main/microservises.png?raw=true)


## Dependencies

This project relies on the following dependencies:
- [P-H-C/phc-winner-argon2](https://github.com/P-H-C/phc-winner-argon2)
- [nlohmann/json](https://github.com/nlohmann/json)
- [boost/asio](https://www.boost.org/doc/libs/1_77_0/doc/html/boost_asio.html)
- [boost/beast](https://www.boost.org/doc/libs/1_77_0/libs/beast/doc/html/index.html)
- [Thalhammer/jwt-cpp](https://github.com/Thalhammer/jwt-cpp)
- [Protobuf](https://developers.google.com/protocol-buffers)
- [gRPC](https://grpc.io/) (and its dependencies)
- [mongocxx](https://mongodb.github.io/mongo-cxx-driver/)
- [bsoncxx](https://mongodb.github.io/mongo-cxx-driver/)
- [Fyne](https://fyne.io/)
- [gorilla/websocket](https://pkg.go.dev/github.com/gorilla/websocket)
- [OpenSSL](https://www.openssl.org/)
- [google/uuid](https://pkg.go.dev/github.com/google/uuid)


## Installation
There are at least two different methods to install and use this project:

1) Build using CMake and Make
   
Before building, make sure to set the ARGON2_PATH and JWT_CPP_PATH variables in the main CMakeLists file.

```cmake
set(ARGON2_PATH "path/to/argon2")
set(JWT_CPP_PATH "path/to/jwt-cpp")
```
Then, proceed with the following commands:
```bash
  make generate
  mkdir -p build
  cd build
  cmake ..
  make
```
Note: Only use the CMakeLists.txt located in the main directory. 
Other CMakeLists (e.g., api_gateway/CMakeLists.txt, etc.) should be used exclusively for building Docker images.

2) Using Docker
```bash
  docker build -t api_gateway:latest -f api_gateway/Dockerfile .
```
Repeat this process for the other components.

Note: If you intend to use Docker in conjunction with Kubernetes, further enhancements may be needed.
