#pragma once

#include <memory>
#include <string>
#include <grpc++/grpc++.h>

#include "DatabaseHandler.grpc.pb.h"

class DBClient
{
public:
    DBClient(const std::string& server_address);

    // Implement methods to send requests to the DatabaseServer
    bool insertUser(const std::string& username, const std::string& password, const std::string& salt);

    bool UserExists(const std::string& username);

    bool storeMessage(const std::string& recipient, const std::string& sender, const std::string& text, std::string& error);

    // std::vector<Message> GetMessages(const std::string& username){
    // }

private:
    std::unique_ptr<Database::Stub> stub_;
    std::shared_ptr<grpc::Channel> channel_;
};