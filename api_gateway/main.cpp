#include <iostream>

#include "Server.hpp"

int main() {
  std::shared_ptr<Server> server = std::make_shared<Server>();
  server->listen();
  server->run();
}
