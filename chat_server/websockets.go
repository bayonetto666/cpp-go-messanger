/*
	Всё +- работает, но не создаётся рум для общения, он для всех общий,
	все сообщения, что приходят, отсылаются всем(то есть и отправителю)
*/

package main

import (
	"fmt"
	"net/http"
	"os"
	"os/signal"
	"sync"

	"github.com/gorilla/websocket"
)

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
}

var (
	connections  = make(map[*Connection]struct{})
	connMutex    sync.Mutex
	messageQueue = make(chan []byte)
)

type Connection struct {
	conn *websocket.Conn
	send chan []byte
}

func main() {
	http.HandleFunc("/chat", handleConnection)
	http.Handle("/", http.FileServer(http.Dir("static")))

	// Сигнал для завершения горутин
	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt)

	go func() {
		<-interrupt
		close(messageQueue)
	}()

	fmt.Println("Server started on :8080")
	http.ListenAndServe(":8080", nil)
}

func handleConnection(w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		fmt.Println(err)
		return
	}

	fmt.Println("Client connected")

	connection := &Connection{
		conn: conn,
		send: make(chan []byte),
	}

	connMutex.Lock()
	connections[connection] = struct{}{}
	connMutex.Unlock()

	go func() {
		defer func() {
			connMutex.Lock()
			delete(connections, connection)
			connMutex.Unlock()
			close(connection.send)
		}()

		for {
			messageType, p, err := conn.ReadMessage()
			if err != nil {
				fmt.Println(err)
				return
			}

			switch messageType {
			case websocket.TextMessage:
				message := string(p)
				fmt.Printf("Received text message: %s\n", message)
				messageQueue <- []byte(message)
			case websocket.BinaryMessage:
				// Обработка бинарных сообщений
			case websocket.CloseMessage:
				// Обработка закрытия соединения
			case websocket.PingMessage:
				// Обработка пинг-сообщений
			case websocket.PongMessage:
				// Обработка понг-сообщений
			}
		}
	}()

	go func() {
		for message := range connection.send {
			if err := conn.WriteMessage(websocket.TextMessage, message); err != nil {
				fmt.Println(err)
				return
			}
		}
	}()

	for {
		select {
		case message := <-messageQueue:
			connMutex.Lock()
			for conn := range connections {
				select {
				case conn.send <- message:
				default:
					close(conn.send)
					delete(connections, conn)
				}
			}
			connMutex.Unlock()
		}
	}
}
