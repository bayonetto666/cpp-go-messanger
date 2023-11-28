#include <iostream>
#include <grpc++/grpc++.h>

#include "DatabaseService.hpp"

int main() {
  std::string server_address("0.0.0.0:50051");
  DatabaseService service("mongodb://localhost:27017");

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();

  return 0;
}
