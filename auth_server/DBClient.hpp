#include <grpc++/grpc++.h>
#include "DatabaseHandler.grpc.pb.h"

class DBClient {
public:
    DBClient(const std::string& server_address) : channel_(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())) {
        stub_ = Database::NewStub(channel_);
    }

    // Implement methods to send requests to the DatabaseServer
    bool insertUser(const std::string& username, const std::string& password, const std::string& salt) {
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

    std::pair<std::string,std::string> GetPassword(const std::string& username, std::string& error){
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

    bool UserExists(const std::string& username){
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
            throw std::runtime_error("Error grpc" + status.error_details());
        }
    }

private:
    std::unique_ptr<Database::Stub> stub_;
    std::shared_ptr<grpc::Channel> channel_;
};