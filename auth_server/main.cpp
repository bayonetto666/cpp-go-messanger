#include <iostream>
#include <grpc++/grpc++.h>
#include <vector>
#include <string>
#include "Auth.hpp"
#include "DatabaseHandler.grpc.pb.h"
#include "DatabaseHandler.pb.h"
#include "AuthServer.grpc.pb.h"
#include "AuthServer.pb.h"

//TODO: fix drop after Error: Failed to get password data: User ___ was not found

/*class AuthServerClient {
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
*/

class AuthenticationServiseImpl final : public Authentication::Service
{
public:
    AuthenticationServiseImpl(const std::string& db_adress, const std::string& secret_key) : _auth(db_adress, secret_key)
    {} 

    grpc::Status authUser(::grpc::ServerContext* context, const ::authUserRequest* request, ::authUserResponse* response)
    override{
        std::string error;
        std::string token = _auth.authUser(request->username(), request->password(), error);

        if (!error.empty())
        {
            return grpc::Status(grpc::StatusCode::UNKNOWN, error);
        }
        response->set_token(std::move(token));
        return grpc::Status::OK;
    }
    
    grpc::Status registerUser(::grpc::ServerContext* context, const ::registerUserRequest* request, ::registerUserResponse* response)
    override{
        //нужно будет доработать как-то
        std::string error;
        bool success = _auth.registerUser(request->username(), request->password(), error);

        if (!success)
        {
            return grpc::Status(grpc::StatusCode::UNKNOWN, error);
        }
        response->set_success(success);

        return grpc::Status::OK;
    }

    grpc::Status getSubject(::grpc::ServerContext* context, const ::getSubjectRequest* request, ::getSubjectResponse* response)
    override{
        std::string error;
        std::string subject = _auth.getSubject(request->token(), error);

        if (!error.empty())
        {
            return grpc::Status(grpc::StatusCode::UNKNOWN, error);
        }
        response->set_subject(std::move(subject));
        return grpc::Status::OK;
    }

    grpc::Status verifyJWT(::grpc::ServerContext* context, const ::verifyJWTRequest* request, ::verifyJWTResponse* response)
    override{
        std::string error;

        bool verified = _auth.verifyJWT(request->token(), error);

        if (!error.empty())
        {
            return grpc::Status(grpc::StatusCode::UNKNOWN, error);
        }
        response->set_verified(verified);
        return grpc::Status::OK;
    }


private:
    Auth _auth;
};



int main() {
    
    std::string server_address("0.0.0.0:50052");
    AuthenticationServiseImpl service("0.0.0.0:50051", "123");
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
   
    server->Wait();


    return 0;
}
