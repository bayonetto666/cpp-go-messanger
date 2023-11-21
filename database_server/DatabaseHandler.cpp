#include "DatabaseHandler.hpp"

DatabaseHandler::DatabaseHandler(const std::string& connection_uri)
{
    mongocxx::instance instance{};
    mongocxx::uri uri(connection_uri);
    _client = mongocxx::client(uri);
    _db = _client["messanger"];
    _usersCollection = _db["users"];
    _messagesCollection = _db["messages"];
}

void DatabaseHandler::insertUser(const nlohmann::json& userData, std::string& error){
    try
    {
        bsoncxx::document::value user_doc = bsoncxx::from_json(userData.dump());

        _usersCollection.insert_one(user_doc.view());
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        error = e.what();
    }
}

bool DatabaseHandler::userExists(const std::string& username, std::string& error){
    try
    {
        bsoncxx::builder::stream::document filter;
        filter << "username" << username;
        auto result = _usersCollection.find(filter.view());
        return result.begin() != result.end();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        error = e.what();
    }
    return false;
}

std::pair<std::string, std::string> DatabaseHandler::getPassword(const std::string &username, std::string& error)
{   
    bsoncxx::builder::stream::document filter;

    filter << "username" << username;
    mongocxx::stdx::optional<bsoncxx::document::value> maybeUser = _usersCollection.find_one(filter.view());
    std::string storedPassword;
    std::string storedSalt;

    if (maybeUser) {
        bsoncxx::document::view user = maybeUser->view();
        if (user.find("password") != user.end() && user.find("salt") != user.end()) {
            storedPassword = user["password"].get_string().value.to_string();
            storedSalt = user["salt"].get_string().value.to_string();            
        } else {
            // Поля "password" и/или "salt" отсутствуют в записи пользователя
            // Обработка ошибки
            error = "Missing fields password and/or salt in userdata";
        }
    } else {
        // Пользователь с заданным именем не найден
        // Обработка ошибки
        error = "User " + username + "was not found";
    }


    return std::make_pair(storedPassword, storedSalt);
}

void DatabaseHandler::storeMessage(const nlohmann::json &message_json, std::string &error)
{
  try
  {
    nlohmann::json updated_message_json = message_json;
    //TODO: fix timezone
    auto current_time = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(current_time);
    std::stringstream formatted_timestamp;
    formatted_timestamp << std::put_time(std::gmtime(&timestamp), "%Y-%m-%d-%H:%M");
    updated_message_json["date"] = formatted_timestamp.str();

    updated_message_json["seen"] = false;

    bsoncxx::document::value message_doc = bsoncxx::from_json(updated_message_json.dump());
    _messagesCollection.insert_one(message_doc.view());
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    error = e.what();
  }
}

nlohmann::json DatabaseHandler::getMessages(const std::string &username, std::string& error)
{
    nlohmann::json messages;
    try
    {
        bsoncxx::builder::stream::document filter;
        filter << "recipient" << username << "seen" << false;

        auto found_messages = _messagesCollection.find(filter.view());

        for (const auto& message : found_messages) {
            nlohmann::json message_json;
            message_json["sender"] = message["sender"].get_string().value.to_string();
            message_json["message"] = message["message"].get_string().value.to_string();
            message_json["date"] = message["date"].get_string().value.to_string();

            markMessageAsRead(message["_id"].get_oid().value.to_string());

            // messages.push_back(std::move(message_json));
            messages.push_back(message_json); //если будут какие-то ошибки

        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        error = e.what();
    }
    return messages;
}

void DatabaseHandler::markMessageAsRead(const std::string& messageId) {
    try {
        bsoncxx::builder::stream::document filter;
        filter << "_id" << bsoncxx::oid(messageId);

        bsoncxx::builder::stream::document update;
        update << "$set" << bsoncxx::builder::stream::open_document
              << "seen" << true
              << bsoncxx::builder::stream::close_document;

        _messagesCollection.update_one(filter.view(), update.view());
    } catch (const std::exception& ex) {
        std::cerr << "Error marking message as read: " << ex.what() << std::endl;
    }
}
