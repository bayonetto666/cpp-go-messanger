#pragma once

#include <memory>
#include <string>
#include <grpc++/grpc++.h>

#include "ChatServer.grpc.pb.h"

class ChatClient
{
public:
  ChatClient(const std::string& server_address);

  [[nodiscard]] std::string createRoom();

private:
  std::unique_ptr<chat::ChatService::Stub> stub_;
  std::shared_ptr<grpc::Channel> channel_;
};