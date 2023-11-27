#include "Server.hpp"

Server::Server()
  : _context(),
    _serverSocket(_context),
    _serverEndpoint(asio::ip::make_address("0.0.0.0"), 8080),
    _acceptor(_context, _serverEndpoint),
    _isRunning(true),
    _db("0.0.0.0:50051"),
    _auth("0.0.0.0:50052"),
    _chat("0.0.0.0:50041") {}

// void Server::stop() {
//     _isRunning = false;
//     _serverSocket.close(); // Закройте серверный сокет, чтобы прервать ожидание accept
// }

void Server::run() {
  _context.run();
}

void Server::listen() {
  std::shared_ptr<asio::ip::tcp::socket> clientSocket = std::make_shared<asio::ip::tcp::socket>(_context);

  _acceptor.async_accept(*clientSocket, [this, clientSocket](const boost::system::error_code ec){
    if (!ec) {
      handleClient(*clientSocket);
      listen();
    } else {
      std::cout << "accept error: " << ec.message() << std::endl;
    }
  });
}

void Server::handleClient(asio::ip::tcp::socket& clientSocket) {
  beast::flat_buffer buffer;
  http::request<http::string_body> request;
  http::request_parser<http::string_body> parser;
  parser.eager(true);
  
  beast::http::read(clientSocket, buffer, parser);
  request = parser.get();
  
  handleRequest(request, clientSocket);
}

void Server::handleRequest(const http::request<http::string_body>& request,asio::ip::tcp::socket& clientSocket) {
  if(request.method() == http::verb::post && request.target() == "/auth/register") {
    handleRegisterRequest(request, clientSocket); //done
    return;
  }
  if(request.method() == http::verb::post && request.target() == "/auth/login") {
    handleLoginRequest(request, clientSocket); //done
    return;
  }
  //надо будет как-то красиво сделать потом
  std::string error;
  std::string token;
  if(request.find(http::field::authorization) == request.end()){
    sendErrorResponse(clientSocket, http::status::bad_request, "Error: request does not contain authorization token", request.version());
    return;
  }
  
  token = request.at(http::field::authorization);

  bool verified = _auth.verifyJWT(token, error);
  if(!error.empty()) {
    sendErrorResponse(clientSocket, http::status::unknown, error, request.version());
    return;
  }
  if (!verified) {
    sendErrorResponse(clientSocket, http::status::unauthorized, "", request.version());
    return;
  }
  if (ws::is_upgrade(request)) {
    if (request.target() == "/chat/new") {
      handleNewChatRequest(request, clientSocket);
    }
    else if (request.target() == "/chat/connect"){
      handleConnectToChatRequest(request, clientSocket);
    }    
  }
  else if (request.method() == http::verb::post && request.target() == "/messages") {
    handleSendMessageRequest(request, clientSocket); //done
  }
  else if (request.method() == http::verb::get && request.target() == "/messages") {
    handleGetMessagesRequest(request, clientSocket); //done
  }
}

void Server::handleSendMessageRequest(const http::request<http::string_body>& request, asio::ip::tcp::socket& clientSocket) {
  std::string error;
  nlohmann::json json_body;

  parseJson(request.body(), json_body, error);
  if (!error.empty()) {
    std::cerr << "Error parsing json, message has not been sent" << std::endl;
    sendErrorResponse(clientSocket, http::status::bad_request, error, request.version());
    return;
  }

  std::string recipient = json_body["recipient"];
  std::string message = json_body["message"];
  std::string token = request.at(http::field::authorization);

  if (!_db.UserExists(recipient))
  {
    sendErrorResponse(clientSocket, http::status::bad_request, "User with name " + recipient + " does not exist", request.version());
    return;
  }
    
  const std::string sender = _auth.getSubject(token, error);

  if (!error.empty()) {      
  }
    
  _db.storeMessage(recipient, sender, message, error);

  http::response<http::string_body> response;
  response.result(http::status::ok);
  response.version(request.version());
  response.set(http::field::server, "Server 0.1");
  response.set(http::field::content_type, "application/json");
  
  if (error.empty()) {
    nlohmann::json resporesponseBody;
    resporesponseBody["success"] = "Message was successfully sent";
    // response.set(http::field::content_length, std::to_string(responseBody.size()));
    response.body() = resporesponseBody.dump();
  } else {
    nlohmann::json resporesponseBody;
    resporesponseBody["success"] = error;
    // std::string responseBody = "Error sending message: " + error;
    response.result(http::status::bad_request);
    // response.set(http::field::content_length, std::to_string(responseBody.size()));
    response.body() = resporesponseBody.dump();
  }
  response.prepare_payload();
  try {
    http::write(clientSocket, response);
  } catch (const std::exception& e) {
    std::cerr << "Error registering user: " << e.what() << std::endl;
    sendErrorResponse(clientSocket, http::status::bad_request, e.what(), 2);
    return;
  }
}

void Server::parseJson(const std::string& body, nlohmann::json& parsedJson, std::string& error) noexcept {
  try {
       parsedJson = nlohmann::json::parse(body);
    } catch (const std::exception& e) {
      error = e.what();
      std::cerr << "Error parsing json: " << e.what() << std::endl;
    }
}

void Server::sendErrorResponse(asio::ip::tcp::socket& clientSocket, const http::status& errorStatus, const std::string& errorMessage, const unsigned short& version) {
  http::response<beast::http::string_body> response;
  response.result(errorStatus);
  response.version(version);
  nlohmann::json body;
  body["error"] = errorMessage;
  response.body() = body.dump();

  http::write(clientSocket, response);
}

void Server::handleRegisterRequest(const http::request<http::string_body> &request, asio::ip::tcp::socket &clientSocket) {
  nlohmann::json json_body;
  std::string error;
  parseJson(request.body(), json_body, error);
  if(!error.empty()){
    std::cerr << "Error parsing json, message has not been sent" << std::endl;
    sendErrorResponse(clientSocket, http::status::bad_request, error, request.version());
    return;
  }
  _auth.registerUser(json_body["username"], json_body["password"], error);        
  if (!error.empty()) {
    sendErrorResponse(clientSocket, http::status::bad_request, error, request.version());
    return;
  }
  
  http::response<http::string_body> response;
  response.result(http::status::ok);
  response.version(request.version());
  response.set(http::field::server, "Server 0.2");
  response.set(http::field::content_type, "application/json");
  nlohmann::json body;
  body["success"] = "Successfuly registered ";
  response.body() = body.dump();

  try {
    http::write(clientSocket, response);
   } catch (const std::exception& e) {
    std::cerr << "Error registering user: " << e.what() << std::endl;
    sendErrorResponse(clientSocket, http::status::bad_request, e.what(), 2);
    return;
   }
}

void Server::handleLoginRequest(const http::request<http::string_body>& request,asio::ip::tcp::socket& clientSocket) {
  nlohmann::json json_body;
  std::string error;
  parseJson(request.body(), json_body, error);
  if(!error.empty()){
    std::cerr << "Error parsing json, login failed" << std::endl;
    sendErrorResponse(clientSocket, http::status::bad_request, error, request.version());
    return;
  }
    
  std::string token = _auth.authUser(json_body["username"], json_body["password"], error);

  if (!error.empty()) {
    sendErrorResponse(clientSocket, http::status::bad_request, error, 2);
  }
    
  http::response<http::string_body> response;
  response.result(http::status::ok);
  response.version(request.version());
  response.set(http::field::server, "Server 0.2");
  response.set(http::field::content_type, "application/json");
  nlohmann::json body;
  body["token"] = token;
  response.body() = body.dump();
  try {
    http::write(clientSocket, response);
  } catch (const std::exception& e) {
    std::cerr << "Error registering user: " << e.what() << std::endl;
    sendErrorResponse(clientSocket, http::status::bad_request, e.what(), request.version());
    return;
  }
}

void Server::handleGetMessagesRequest(const http::request<http::string_body>& request, asio::ip::tcp::socket& clientSocket) {
  http::response<http::string_body> response;
  response.result(http::status::ok);
  response.version(request.version());
  response.set(http::field::server, "Server 0.1");
  response.set(http::field::content_type, "application/json");

  std::string token = request.at(http::field::authorization);
  std::string error;
  auto username = _auth.getSubject(token, error);
  auto messages = _db.GetMessages(username);
  for(const auto& message : messages){
    nlohmann::json msg;
    msg["sender"] = message.sender();
    msg["text"] = message.text();
    response.body().append(std::move(msg.dump()));
  }

  try {
    http::write(clientSocket, response);
  } catch (const std::exception& e) {
    std::cerr << "Error registering user: " << e.what() << std::endl;
    sendErrorResponse(clientSocket, http::status::bad_request, e.what(), request.version());
    return;
  }
}

void Server::handleNewChatRequest(const http::request<http::string_body> &request, asio::ip::tcp::socket &clientSocket) {
  std::string error;
  auto username = _auth.getSubject(request.at(http::field::authorization), error);

  auto room_id = _chat.createRoom();
  std::cout << "Created room " << room_id << std::endl;
      
  if(request.find("invited_users") == request.end()){
    sendErrorResponse(clientSocket, http::status::bad_request, "Error: request does not contain invited users", request.version());
    return;
  }

  auto invitedUsersHeader = request.at("invited_users");
  std::vector<std::string> invitedUsers;
  boost::split(invitedUsers, invitedUsersHeader, boost::is_any_of(","));
      
  inviteUsers(username, room_id, invitedUsers);

  if(!error.empty()){
    sendErrorResponse(clientSocket, http::status::internal_server_error, "", request.version());
  }
  auto clientWebSocketPtr = std::make_shared<ws::stream<tcp::socket>>(std::move(clientSocket));
  clientWebSocketPtr->accept(request);

  handleWebSocketConnection(*clientWebSocketPtr, username, room_id);
}

void Server::handleConnectToChatRequest(const http::request<http::string_body>& request, asio::ip::tcp::socket& clientSocket){
  auto room_id = request.at("room_id");
  std::string error;
  auto username = _auth.getSubject(request.at(http::field::authorization), error);
  if(!error.empty()){
    sendErrorResponse(clientSocket, http::status::internal_server_error, "", request.version());
  }
  auto clientWebSocketPtr = std::make_shared<ws::stream<tcp::socket>>(std::move(clientSocket));
  clientWebSocketPtr->accept(request);
  
  handleWebSocketConnection(*clientWebSocketPtr, username, room_id);
}


void Server::handleWebSocketConnection(ws::stream<tcp::socket>& clientWs, const std::string username, const std::string room_id) {
  try {
    tcp::socket serverSocket(_context);
    
    serverSocket.connect({ip::make_address("0.0.0.0"), 50010});
    auto serverWs = std::make_shared<ws::stream<tcp::socket>>(std::move(serverSocket));
    serverWs->handshake("0.0.0.0:50010", "/chat?room_id=" + room_id);
    
    auto proxy = std::make_shared<websocket_proxy>(_context, clientWs, *serverWs, username);      
    
    proxy->run();
    
  } catch (beast::system_error const& se) {
    if (se.code() != beast::websocket::error::closed)
      std::cerr << "Error: " << se.code().message() << std::endl;
  } catch (std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void Server::inviteUsers(const std::string& inviter, const std::string& room_id ,const std::vector<std::string> invitedUsers) {
  //TODO: add date of invitation
  std::string message =  "user " + inviter + " invited you to chat " + room_id;
  std::string error;
  for(const auto user : invitedUsers){
    _db.storeMessage(user, inviter, message, error);
    if(!error.empty()){
      std::cout << "Error inviting user " << user << " to room " << room_id + ": " << error << std::endl;
    }
  }  
}
