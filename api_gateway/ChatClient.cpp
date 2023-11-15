#include "ChatClient.hpp"

ChatClient::ChatClient(const std::string &server_address)  : channel_(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())) {
        stub_ = chat::ChatService::NewStub(channel_); 
}

std::string ChatClient::createRoom()
{
    chat::RoomRequest request;

    std::string room_id = "123"; //нужно будет добавить генерацию id
    request.set_room_id(room_id); 

    chat::RoomResponse response;
    grpc::ClientContext context;

    grpc::Status status = stub_->CreateRoom(&context, request, &response);

    if (!status.ok())
    {
        /* code */
    }
    
    return room_id;
}