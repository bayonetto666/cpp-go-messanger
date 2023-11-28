#pragma once

#include <memory>
#include <string>
#include <grpc++/grpc++.h>

#include "AuthServer.grpc.pb.h"

class AuthClient
{
public:
  AuthClient(const std::string& server_address);

  std::string authUser(const std::string& username, const std::string& password, std::string& error);

  bool registerUser(const std::string& username, const std::string& password, std::string& error);

  bool verifyJWT(const std::string& token, std::string& error);

  std::string getSubject(const std::string& token, std::string& error);
private:
  std::unique_ptr<Authentication::Stub> stub_;
  std::shared_ptr<grpc::Channel> channel_;
};