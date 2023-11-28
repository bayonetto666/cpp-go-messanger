#include "ChatClient.hpp"

ChatClient::ChatClient(const std::string &server_address)  : channel_(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())) {
  stub_ = chat::ChatService::NewStub(channel_); 
}

std::string ChatClient::createRoom()
{
  google::protobuf::Empty request;

  chat::RoomResponse response;
  grpc::ClientContext context;

  grpc::Status status = stub_->CreateRoom(&context, request, &response);
    
  std::string room_id = response.room_id();

  if (!status.ok()) {
    std::cout << "createRoom status error: " << status.error_message() << std::endl;
  }
  if (!response.error().empty()) {
    std::cout << "createRoom generating error: " << response.error() << std::endl;
  }
    
  return room_id;
}