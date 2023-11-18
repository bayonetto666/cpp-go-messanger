#include "websocket_proxy.hpp"

websocket_proxy::websocket_proxy(asio::io_context& context, ws::stream<tcp::socket>& clientWs, ws::stream<tcp::socket>& serverWs) :
_context(context), _clientWs(std::move(clientWs)), _serverWs(std::move(serverWs))
{}

void websocket_proxy::run(){
  _readFromClient();
  _readFromServer();
}

void websocket_proxy::close() {
  boost::system::error_code ec;
  if (_clientWs.is_open()) {
    std::cout << "closing _clientWs\n";
    _clientWs.close(beast::websocket::close_code::normal, ec);
  } else {
    std::cout << "_clientWs is already closed \n";
  }
  if (_serverWs.is_open()) {
    std::cout << "closing _serverWs\n";
    _serverWs.close(beast::websocket::close_code::normal, ec);
  } else {
    std::cout << "_serverWs is already closed \n";
  }
}

void websocket_proxy::_readFromServer(){
  _serverBuffer.consume(_serverBuffer.size());
  _serverWs.async_read(_serverBuffer, 
    [self = shared_from_this()](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      if (!ec) {
        self->_writeToClient();
      } else {
        std::cout << "Error reading from server: "<< ec.message() << std::endl;
        self->close();
      }
    }
  );
}

void websocket_proxy::_readFromClient(){
  _clientBuffer.consume(_clientBuffer.size());
  _clientWs.async_read(_clientBuffer,
    [self = shared_from_this()](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      if (!ec) {
        self->_writeToServer();
      } else {
        std::cout << "Error reading from client: "<< ec.message() << std::endl;
        self->close();
      }
    }
  );
}

void websocket_proxy::_writeToServer(){
  auto buffer = std::make_shared<beast::flat_buffer>(_clientBuffer);
  _serverWs.async_write(buffer->data(),
    [self = shared_from_this(), buffer](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      if (!ec) {
        self->_readFromClient();
      } else {
        std::cout << "Error writing to server: "<< ec.message() << std::endl;
        self->close();
      }
    }
  );
}

void websocket_proxy::_writeToClient(){
  auto buffer = std::make_shared<beast::flat_buffer>(_serverBuffer);
  _clientWs.async_write(buffer->data(),
    [self = shared_from_this(), buffer](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      if (!ec) {
        self->_readFromServer();
      } else {
        std::cout << "Error writing to client: "<< ec.message() << std::endl;
        self->close();
      }
    }
  );
}