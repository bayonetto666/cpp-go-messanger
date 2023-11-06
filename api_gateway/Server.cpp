#include "Server.hpp"

Server::Server() : _context(), _serverSocket(_context),
    _serverEndpoint(asio::ip::make_address("127.0.0.1"), 8080), _acceptor(_context, _serverEndpoint), _isRunning(true)
//,    _dbHandler(std::make_shared<DatabaseHandler>("mongodb://localhost:27017"))
{}

void Server::stop() {
    _isRunning = false;
    _serverSocket.close(); // Закройте серверный сокет, чтобы прервать ожидание accept
}

void Server::listen(){
    while (_isRunning) {
        asio::ip::tcp::socket clientSocket(_context); // Создаем сокет для клиента
        asio::ip::tcp::endpoint clientEndpoint;
        
        // Принимаем входящее соединение
        _acceptor.accept(clientSocket, clientEndpoint);
        
        auto clientSocketPtr = std::make_shared<asio::ip::tcp::socket>(std::move(clientSocket));

        std::thread([this, clientSocketPtr, &clientEndpoint]() {
            handleClient(*clientSocketPtr, clientEndpoint);
        }).detach();
    }
}

void Server::handleRequest(const http::request<http::string_body>& request,asio::ip::tcp::socket& clientSocket,
    const asio::ip::tcp::endpoint& clientEndpoint) {
    if (request.method() == http::verb::post && request.target() == "/messages") {
        handleSendMessageRequest(request, clientSocket, clientEndpoint);
    }
    else if (request.method() == http::verb::get && request.target() == "/messages") {
        handleGetMessagesRequest(request, clientSocket);
    }
    else if(request.method() == http::verb::post && request.target() == "/auth/register") {
        handleRegisterRequest(request, clientSocket);
    }
    else if(request.method() == http::verb::post && request.target() == "/auth/login") {
        handleLoginRequest(request, clientSocket);
    }
}

void Server::handleSendMessageRequest(const http::request<http::string_body>& request, asio::ip::tcp::socket& clientSocket, const asio::ip::tcp::endpoint& clientEndpoint) {
//    std::string ex_what;
//    nlohmann::json json_body;
//
//    if (!parseJson(request.body(), json_body, ex_what)) {
//        std::cerr << "Error parsing json, message has not been sent" << std::endl;
//        sendErrorResponse(clientSocket, http::status::bad_request, ex_what, request.version());
//        return;
//    }
//    std::string recipient;
//    std::string message;
//    std::string token;
//    recipient = json_body["recipient"];
//    message = json_body["message"];
//    try {
//        token = request.at(http::field::authorization);
//    } catch (const std::exception& e) {
//        std::cout << "Error: " << e.what() << std::endl;
//        sendErrorResponse(clientSocket, http::status::bad_request, "Error: request does not contain authorization token", request.version());
//        return;
//    }
//
//    if (!AuthJWT::verifyJWT(token, _secretKey)) {
//        sendErrorResponse(clientSocket, http::status::unauthorized, "Token was not verified", request.version());
//        return;
//    }
//    
//    if (!_dbHandler->userExists(recipient)) {
//        sendErrorResponse(clientSocket, http::status::bad_request, "User with name " + recipient + " does not exist", request.version());
//        return;
//    }
//
//    const std::string sender = AuthJWT::getSubject(token, _secretKey);
//    
//    _dbHandler->storeMessage(sender, recipient, message, ex_what);
//
//    http::response<http::string_body> response;
//    response.result(http::status::ok);
//    response.version(request.version());
//    response.set(http::field::server, "Server 0.1");
//    response.set(http::field::content_type, "application/json");
//    
//    if (ex_what.empty()) {
//        std::string responseBody = "Message was successfully sent";
//        response.set(http::field::content_length, std::to_string(responseBody.size()));
//        response.body() = std::move(responseBody);
//    } else {
//        std::string responseBody = "Error sending message: " + ex_what;
//        response.result(http::status::bad_request);
//        response.set(http::field::content_length, std::to_string(responseBody.size()));
//        response.body() = std::move(responseBody);
//    }
//
//    response.prepare_payload();
//    http::write(clientSocket, response);
}

bool Server::parseJson(const std::string& body, nlohmann::json& parsedJson, std::string& ex_what){
    try {
            parsedJson = nlohmann::json::parse(body);
            return true; // JSON был успешно разобран
        } catch (const std::exception& e) {
            ex_what = e.what();
            std::cerr << "Json parsing error : " << e.what() << std::endl;
            return false; // Возникла ошибка при парсинге JSON
        }
}

//void Server::sendMessage(asio::ip::tcp::socket& recipient_socket, const std::string& message, const std::string& sender){
//
//    // Формирование POST-запроса
//    http::request<http::string_body> post_request;
//    post_request.method(http::verb::post);
//    post_request.target("/messages");
//    post_request.version(11);
//    post_request.set(beast::http::field::host, "client.com");
//    post_request.set(beast::http::field::content_type, "application/json");
//
//    nlohmann::json post_data = {
//        {"sender", sender},
//        {"message", message}
//    };
//    post_request.body() = post_data.dump();
//    post_request.prepare_payload();
//
//    // Отправка POST-запроса получателю
//    http::write(recipient_socket, post_request);
//}

void Server::sendErrorResponse(asio::ip::tcp::socket& clientSocket, const http::status& errorStatus, const std::string& errorMessage, const unsigned short& version){
    http::response<beast::http::string_body> response;
    response.result(errorStatus);
    response.version(version);
    std::string body = "Error occured:\n" + errorMessage;
    response.body().append(body);

    http::write(clientSocket, response);
}

void Server::handleClient(asio::ip::tcp::socket& clientSocket, const asio::ip::tcp::endpoint& clientEndpoint){
    // Теперь можно читать данные из clientSocket
    beast::flat_buffer buffer;
    http::request<http::string_body> request;
    http::request_parser<http::string_body> parser;
    parser.eager(true);
    
    beast::http::read(clientSocket, buffer, parser);
    request = parser.get();
    
    handleRequest(request, clientSocket, clientEndpoint);
}
void Server::handleRegisterRequest(const http::request<http::string_body>& request,asio::ip::tcp::socket& clientSocket){
//    nlohmann::json json_body;
//    std::string ex_what;
//    if(!parseJson(request.body(), json_body, ex_what)){
//        std::cerr << "Error parsing json, message has not been sent" << std::endl;
//        sendErrorResponse(clientSocket, http::status::bad_request, ex_what, request.version());
//        return;
//    }
//    try {
//        _dbHandler->insertUser(json_body);
//    } catch (const std::exception& e) {
//        std::cerr << "Error registering user: " << e.what() << std::endl;
//        sendErrorResponse(clientSocket, http::status::bad_request, e.what(), 11);
//        return;
//    }
//    
//    http::response<http::string_body> response;
//    response.result(http::status::ok);
//    response.version(request.version());
//    response.set(http::field::server, "YourServerName");
//    response.set(http::field::content_type, "application/json");
//    response.set(http::field::access_control_allow_origin, "*");  // Это заголовок CORS для разрешения запросов с любого источника

    // Если вы хотите добавить дополнительные заголовки, например, заголовок для длины содержимого (Content-Length), вы можете сделать это так:
    // response.set(http::field::content_length, std::to_string(responseBody.size()));  // responseBody - ваш ответ в виде строки

    // Если у вас есть тело ответа (response body), добавьте его следующим образом:
//    std::string responseBody = "Registration successful";  // Ваше сообщение об успешной регистрации
//    response.body() = responseBody;
//
//    // Укажите длину содержимого (Content-Length), если это необходимо
//    response.set(http::field::content_length, std::to_string(responseBody.size()));
//
//    response.prepare_payload();  // Подготовьте ответ для отправки
//
//    http::write(clientSocket, response);

}

void Server::handleLoginRequest(const http::request<http::string_body>& request,asio::ip::tcp::socket& clientSocket){
//    nlohmann::json json_body;
//    std::string ex_what;
//    if(!parseJson(request.body(), json_body, ex_what)){
//        std::cerr << "Error parsing json, message has not been sent" << std::endl;
//        sendErrorResponse(clientSocket, http::status::bad_request, ex_what, request.version());
//        return;
//    }
//    std::string username = json_body["username"];
//    if(!(_dbHandler->userExists(username))){
//        sendErrorResponse(clientSocket, http::status::unauthorized, "Wrong username", 11);
//        return;
//    }
//    if(_dbHandler->verifyPassword(json_body["username"], json_body["password"])){
//        
//        auto token = AuthJWT::generateJWT("Server 0.1", json_body["username"], _secretKey);
//        http::response<http::string_body> response;
//        response.result(http::status::ok);
//        response.version(request.version());
//        response.set(http::field::server, "Server 0.1");
//        response.set(http::field::content_type, "application/json");
//        std::string responseBody = "Login successful\n" + token;  // Ваше сообщение об успешной регистрации
//        response.body() = responseBody;
//        // Укажите длину содержимого (Content-Length), если это необходимо
//        response.set(http::field::content_length, std::to_string(responseBody.size()));
//
//        response.prepare_payload();  // Подготовьте ответ для отправки
//
//        http::write(clientSocket, response);
//    }
//    else
//        sendErrorResponse(clientSocket, http::status::unauthorized, "Wrong password", 11);
//    
}

void Server::handleGetMessagesRequest(const http::request<http::string_body>& request, asio::ip::tcp::socket& clientSocket){
//    try {
//        std::string ex_what;
//    //    nlohmann::json json_body;
//    //
//    //    if (!parseJson(request.body(), json_body, ex_what)) {
//    //        std::cerr << "Error parsing json, message has not been sent" << std::endl;
//    //        sendErrorResponse(clientSocket, http::status::bad_request, ex_what, request.version());
//    //        return;
//    //    }
//        
//        std::string token = request.at(http::field::authorization);
//        
//        auto username = AuthJWT::getSubject(token, _secretKey);
//        
//        auto messages = _dbHandler->getMessages(username);
//        
//        http::response<http::string_body> response;
//        response.result(http::status::ok);
//        response.version(request.version());
//        response.set(http::field::server, "Server 0.1");
//        response.set(http::field::content_type, "application/json");
//        
//        std::string messages_json = messages.dump();
//        response.set(http::field::content_length, std::to_string(messages_json.size()));
//        response.body() = std::move(messages_json);
//
//        http::write(clientSocket, response);
//    } catch (const std::exception& ex) {
//        std::cerr << ex.what() << std::endl;
//        sendErrorResponse(clientSocket, http::status::bad_request, ex.what(), 11);
//    }
}
