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
  
  if (status.ok())
  {
    return response.exists();
  }
  else{
  
  }
}

bool DBClient::storeMessage(const std::string& recipient, const std::string& sender, const std::string& text, std::string& error){
  Message message;
  message.set_recipient(recipient);
  message.set_sender(sender);
  message.set_text(text);
  
  StoreMessageResponse response;
  grpc::ClientContext context;
  grpc::Status status = stub_->StoreMessage(&context, message, &response);
  
  if (status.ok())
  {
    return response.success();
  }
}

std::vector<Message> DBClient::GetMessages(const std::string &username) {
  // Создание объекта запроса
  GetMessagesRequest request;
  request.set_user(username);
  std::vector<Message> messages;
  // Создание объекта контекста
  grpc::ClientContext context;

  // Создание объекта для приема данных от сервера
  std::unique_ptr<grpc::ClientReader<Message>> reader(stub_->GetMessages(&context, request));

  // Чтение данных от сервера
  Message message;
  while (reader->Read(&message)) {
    // Обработка полученного сообщения
    std::cout << "Received message from " << message.sender() << ": " << message.text() << std::endl;
    messages.push_back(message);
  }

  // Получение статуса вызова
  grpc::Status status = reader->Finish();
  if (status.ok()) {
    std::cout << "GetMessages RPC succeeded." << std::endl;
  } else {
    std::cerr << "GetMessages RPC failed with status code " << status.error_code() << ": " << status.error_message() << std::endl;
  }
  return messages;
}
