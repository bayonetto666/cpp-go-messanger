#pragma once

#include <grpc++/grpc++.h>
#include <vector>
#include <string>
#include "Auth.hpp"
#include "DatabaseHandler.grpc.pb.h"
#include "DatabaseHandler.pb.h"
#include "AuthServer.grpc.pb.h"
#include "AuthServer.pb.h"


class AuthService final : public Authentication::Service
{
public:
    AuthService(const std::string& db_address, const std::string& secret_key) : _auth(db_address, secret_key)
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

        if (!success) {
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

