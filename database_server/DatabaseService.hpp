#pragma once

#include <nlohmann/json.hpp>
#include "DatabaseHandler.hpp"
#include <grpc++/grpc++.h>
#include "DatabaseHandler.grpc.pb.h"

#include "DatabaseHandler.pb.h"

class DatabaseService final : public Database::Service{
public:

  DatabaseService(const std::string& db_address): dbh(db_address)
  {} 

  grpc::Status InsertUser(::grpc::ServerContext* context, const ::InsertUserRequest* request, ::InsertUserResponse* response) override {
    std::string error;
    nlohmann::json user_data;
    
    user_data["username"] = request->username();
    user_data["password"] = request->password();
    user_data["salt"] = request->salt();
    
    dbh.insertUser(user_data, error);
    
    if (!error.empty()) {
      response->set_success(false);
      response->set_error(error);
      return grpc::Status(grpc::StatusCode::UNKNOWN, error);
    }
    
    response->set_success(true);
    return grpc::Status::OK;
  }
    
  grpc::Status UserExists(::grpc::ServerContext* context, const ::UserExistsRequest* request, ::UserExistsResponse* response) override {
    std::string error;
    bool exists = dbh.userExists(request->user(), error);
    
    if (!error.empty()) {
      return grpc::Status(grpc::StatusCode::UNKNOWN, error);
    }
    
    response->set_exists(exists);
    return grpc::Status::OK;
  }
    
   grpc::Status StoreMessage(::grpc::ServerContext* context, const ::Message* request, ::StoreMessageResponse* response) override {
    std::string error;
    nlohmann::json messageData;
      
    messageData["sender"] = request->sender();
    messageData["recipient"] = request->recipient();
    messageData["text"] = request->text();

    dbh.storeMessage(messageData, error);

    if (!error.empty()) {
      response->set_success(false);
      response->set_error(error);
      return grpc::Status(grpc::StatusCode::UNKNOWN, error);
    }
    response->set_success(true);
    return grpc::Status::OK;
}

grpc::Status GetMessages(::grpc::ServerContext* context, const ::GetMessagesRequest* request, ::grpc::ServerWriter< ::Message>* writer) override {
  std::string error;
  
  nlohmann::json messages = dbh.getMessages(request->user(), error);
  if (!error.empty()) {
    return grpc::Status(grpc::StatusCode::UNKNOWN, error);
  }
  
  for (const auto& message : messages) {
    ::Message response_message;
    response_message.set_sender(message["sender"].get<std::string>());
    response_message.set_text(message["text"].get<std::string>());
  
    if (!writer->Write(response_message)) {
      return grpc::Status(grpc::StatusCode::UNKNOWN, "Failed to write messages to the client");
    }
  }
  
  return grpc::Status::OK;
}


  grpc::Status GetPassword(::grpc::ServerContext* context, const ::GetPasswordRequest* request, ::GetPasswordResponse* response) override {
    std::string error;
    auto password_data = dbh.getPassword(request->username(), error);
    
    if (!error.empty()) {
      return grpc::Status(grpc::StatusCode::UNKNOWN, "Failed to get password data: " + error);
    }
    
    response->set_hashed_password(password_data.first);
    response->set_salt(password_data.second);
    
    return grpc::Status::OK;
    }

private:
    DatabaseHandler dbh;    
};