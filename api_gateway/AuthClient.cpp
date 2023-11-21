#include "AuthClient.hpp"

AuthClient::AuthClient(const std::string& server_address) : channel_(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())) {
        stub_ = Authentication::NewStub(channel_); 
}

std::string AuthClient::authUser(const std::string& username, const std::string& password, std::string& error) {
        authUserRequest request;
        request.set_username(username);
        request.set_password(password);
        authUserResponse response;
        grpc::ClientContext context;
        grpc::Status status = stub_->authUser(&context, request, &response);
        
        if (!status.ok())
        {

        }
        if (!response.error().empty())
        {

        }
        return response.token();
    }

bool AuthClient::registerUser(const std::string& username, const std::string& password, std::string& error) {
        registerUserRequest request;
        request.set_username(username);
        request.set_password(password);
        registerUserResponse response;
        grpc::ClientContext context;
        grpc::Status status = stub_->registerUser(&context, request, &response);
        
        if (!status.ok()) {
          
        }

        return response.success();
    }

bool AuthClient::verifyJWT(const std::string& token, std::string& error) {

        verifyJWTRequest request;
        request.set_token(token);
        verifyJWTResponse response;
        grpc::ClientContext context;
        grpc::Status status = stub_->verifyJWT(&context, request, &response);
        if (!status.ok())
        {

        }
        if (!response.error().empty())
        {

        }
        return response.verified();
    }

std::string AuthClient::getSubject(const std::string& token, std::string& error) {
        getSubjectRequest request;
        request.set_token(token);
        getSubjectResponse response;
        grpc::ClientContext context;
        grpc::Status status = stub_->getSubject(&context, request, &response);
        if (!status.ok())
        {

        }
        if (!response.error().empty())
        {

        }
        return response.subject();
    }