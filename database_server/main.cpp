#include <iostream>
#include <nlohmann/json.hpp>
#include "DatabaseHandler.hpp"
#include <grpc++/grpc++.h>
#include "DatabaseHandler.grpc.pb.h"

#include "DatabaseHandler.pb.h"

class DatabaseServiceImpl final : public Database::Service{
public:

    // DatabaseServiceImpl(const std::string& DatabaseAdress) : dbh(DatabaseAdress)
    // {} 
    DatabaseServiceImpl(): dbh("mongodb://localhost:27017")
    {} 

    grpc::Status InsertUser(::grpc::ServerContext* context, const ::InsertUserRequest* request, ::InsertUserResponse* response)
    override
    {
        std::string error;
        nlohmann::json user_data;
        user_data["username"] = request->username();
        user_data["password"] = request->password();
        user_data["salt"] = request->salt();

        dbh.insertUser(user_data, error);

        if (!error.empty()) {
            response->set_success(false);
            response->set_error(error);
            return grpc::Status(grpc::StatusCode::UNKNOWN, error);
        }
        response->set_success(true);

        return grpc::Status::OK;
    }
    
    grpc::Status UserExists(::grpc::ServerContext* context, const ::UserExistsRequest* request, ::UserExistsResponse* response) override {
        std::string error;
        bool exists = dbh.userExists(request->user(), error);

        if (!error.empty()) {
            return grpc::Status(grpc::StatusCode::UNKNOWN, error);
        }

        response->set_exists(exists);

        return grpc::Status::OK;
    }
    
   grpc::Status StoreMessage(::grpc::ServerContext* context, const ::Message* request, ::StoreMessageResponse* response) override {
    std::string error;
    nlohmann::json messageData;
    // Iterate over the fields in the request and add them to the JSON object.
    const google::protobuf::Reflection* reflection = request->GetReflection();
    const google::protobuf::Descriptor* descriptor = request->GetDescriptor();
    for (int i = 0; i < descriptor->field_count(); ++i) {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);
        const std::string& field_name = field->name();

        // Use reflection to get the field's value.
        if (reflection->HasField(*request, field)) {
            switch (field->cpp_type()) {
                case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                    messageData[field_name] = reflection->GetInt32(*request, field);
                    break;
                case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                    messageData[field_name] = reflection->GetString(*request, field);
                    break;
                // Handle other field types as needed.
                default:
                    // Handle unsupported field types or skip them.
                    break;
            }
        }
    }

    // Call the database handler to store the message using the JSON data.
    dbh.storeMessage(messageData, error);

    if (!error.empty()) {
        response->set_success(false);
        response->set_error(error);
        return grpc::Status(grpc::StatusCode::UNKNOWN, error);
    }
    response->set_success(true);
    return grpc::Status::OK;
}

    grpc::Status GetMessages(::grpc::ServerContext* context, const ::GetMessagesRequset* request, ::grpc::ServerWriter< ::Message>* writer) override {
        std::string error;

        // Retrieve the messages from the database.
        nlohmann::json messages = dbh.getMessages(request->user(), error);

        if (!error.empty()) {
            return grpc::Status(grpc::StatusCode::UNKNOWN, error);
        }

        // Iterate over the messages and send them to the client.
        for (const auto& message : messages) {
            ::Message response_message;
            // Populate response_message fields based on the message JSON.
            response_message.set_sender(message["sender"].get<std::string>());
            response_message.set_recipient(message["recipient"].get<std::string>());
            response_message.set_text(message["text"].get<std::string>());


            if (!writer->Write(response_message)) {
                // If writing fails, return an error.
                return grpc::Status(grpc::StatusCode::UNKNOWN, "Failed to write messages to the client");
            }
        }

        return grpc::Status::OK;
    }

private:
    DatabaseHandler dbh;    
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    DatabaseServiceImpl service;
    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main() {
    // std::string server_address("0.0.0.0:50051");
    // DatabaseServiceImpl service("mongodb://localhost:27017");

    // grpc::ServerBuilder builder;
    // builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // builder.RegisterService(&service);
    // std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    // std::cout << "Server listening on " << server_address << std::endl;
    // server->Wait();

    RunServer();

    return 0;
}
