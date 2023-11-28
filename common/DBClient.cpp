#include "DBClient.hpp"

DBClient::DBClient(const std::string& server_address) : channel_(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())) {
  stub_ = Database::NewStub(channel_);
}


bool DBClient::insertUser(const std::string& username, const std::string& password, const std::string& salt) {
  InsertUserRequest request;
  request.set_username(username);
  request.set_password(password);
  request.set_salt(salt);
  InsertUserResponse response;
  grpc::ClientContext context;
  grpc::Status status = stub_->InsertUser(&context, request, &response);
  
  if (status.ok()) {
    return true;
  } else {
    std::cerr << "Error: " << status.error_message() << std::endl;
    return false;
  }
}

bool DBClient::UserExists(const std::string& username){
  UserExistsRequest request;
  request.set_user(username);
  UserExistsResponse response;
  grpc::ClientContext context;
  grpc::Status status = stub_->UserExists(&context, request, &response);
  
  if (!status.ok()) {
    
  }
  return response.exists();
}

bool DBClient::storeMessage(const std::string& recipient, const std::string& sender, const std::string& text, std::string& error){
  Message message;
  message.set_recipient(recipient);
  message.set_sender(sender);
  message.set_text(text);
  
  StoreMessageResponse response;
  grpc::ClientContext context;
  grpc::Status status = stub_->StoreMessage(&context, message, &response);
  
  if (!status.ok()) {
    return false;
  }
  return response.success();
}

std::vector<Message> DBClient::GetMessages(const std::string &username) {
  GetMessagesRequest request;
  request.set_user(username);
  std::vector<Message> messages;
  grpc::ClientContext context;

  std::unique_ptr<grpc::ClientReader<Message>> reader(stub_->GetMessages(&context, request));

  Message message;
  while (reader->Read(&message)) {
    std::cout << "Received message from " << message.sender() << ": " << message.text() << std::endl;
    messages.push_back(message);
  }

  grpc::Status status = reader->Finish();
  if (status.ok()) {
    std::cout << "GetMessages RPC succeeded." << std::endl;
  } else {
    std::cerr << "GetMessages RPC failed with status code " << status.error_code() << ": " << status.error_message() << std::endl;
  }
  return messages;
}

std::pair<std::string,std::string> DBClient::GetPassword(const std::string& username, std::string& error){
  grpc::ClientContext context;
  GetPasswordRequest request;
  request.set_username(username);
  GetPasswordResponse response;
  grpc::Status status = stub_->GetPassword(&context, request, &response);
        
  if (status.ok()) {
    return std::make_pair(response.hashed_password(), response.salt());
  } else {
    std::cerr << "Error: " << status.error_message() << std::endl;
    error = status.error_message();
    return std::make_pair("", "");
  }
}

