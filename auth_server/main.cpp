#include <iostream>
#include <grpc++/grpc++.h>
#include <vector>
#include "Auth.hpp"
#include "DatabaseHandler.grpc.pb.h"
#include "DatabaseHandler.pb.h"

class AuthServerClient {
public:
    AuthServerClient(const std::string& server_address) : channel_(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())) {
        stub_ = Database::NewStub(channel_);
    }

    // Implement methods to send requests to the DatabaseServer
    bool insertUser(const std::string& username, const std::string& password) {
        InsertUserRequest request;
        request.set_username(username);
        request.set_password(password);
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

    bool UserExists(const std::string& username){
        UserExistsRequest request;
        request.set_user(username);
        UserExistsResponse response;
        grpc::ClientContext context;
        grpc::Status status = stub_->UserExists(&context, request, &response);

        if (status)
        {
            return response.exists();
        }
        else{
            throw std::runtime_error("Error grpc");
        }
    }
    // // void StoreMessage(){}

    // std::vector<Message> GetMessages(const std::string& username){
    //     //GetMessagesRequest request
    //     GetMessagesRequset request; //нужно будет пофиксить в прото файле
    //     request.set_user(username);
        
    //     std::vector<Message> messages;
    //     while (reader->Read(&message)) {
    //     messages.push_back(message);
    //     }
    //     // Далее, отправьте сообщения на API Gateway.
    //     // if (!messages.empty()) {

    //     // }
    //     return messages;
    // }

   


private:
    std::unique_ptr<Database::Stub> stub_;
    std::shared_ptr<grpc::Channel> channel_;
};


int main() {
    // try
    // {
    //     std::string server_address = "0.0.0.0:50051"; // Update with the actual address of the DatabaseServer
    //     AuthServerClient client(server_address);

    //     // Example usage of the client
    //     std::string username = "user123";
    //     std::string password = "password123";

    //     auto res = client.insertUser(username, password);

    //     if (res) {
    //         std::cout << "User inserted successfully." << std::endl;
    //     } else {
    //         std::cerr << "Failed to insert user." << std::endl;
    //     }
    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << e.what() << '\n';
    // }
    
    return 0;
}
