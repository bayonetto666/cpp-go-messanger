#pragma once

#include <memory>
#include <string>
#include <grpc++/grpc++.h>

#include "DatabaseHandler.grpc.pb.h"

class DBClient
{
public:
  DBClient(const std::string& server_address);

  bool insertUser(const std::string& username, const std::string& password, const std::string& salt);

  bool UserExists(const std::string& username);

  bool storeMessage(const std::string& recipient, const std::string& sender, const std::string& text, std::string& error);

  std::vector<Message> GetMessages(const std::string& username);

  std::pair<std::string,std::string> GetPassword(const std::string& username, std::string& error);

private:
  std::unique_ptr<Database::Stub> stub_;
  std::shared_ptr<grpc::Channel> channel_;
};