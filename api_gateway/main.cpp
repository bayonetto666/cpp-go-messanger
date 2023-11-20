#include <iostream>

#include "Server.hpp"

int main() {
  std::shared_ptr<Server> server = std::make_shared<Server>();
  // std::thread([&server]() {
  //   server->listen();
  // }).detach();
  server->listen();

    // Run the IO context in the main thread
    // server->run();
}
