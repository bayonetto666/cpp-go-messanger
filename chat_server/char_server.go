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

type ChatRoom struct {
	ID           string
	connections  map[*Connection]struct{}
	connMutex    sync.Mutex
	messageQueue chan []byte
}

func NewChatRoom(roomID string) *ChatRoom {
	return &ChatRoom{
		ID:           roomID,
		connections:  make(map[*Connection]struct{}),
		messageQueue: make(chan []byte),
	}
}

func (c *ChatRoom) AddConnection(conn *Connection) {
	c.connMutex.Lock()
	defer c.connMutex.Unlock()
	c.connections[conn] = struct{}{}
}

func (c *ChatRoom) RemoveConnection(conn *Connection) {
	c.connMutex.Lock()
	defer c.connMutex.Unlock()
	delete(c.connections, conn)
	close(conn.send)
}

func (c *ChatRoom) BroadcastMessage(message []byte) {
	c.connMutex.Lock()
	defer c.connMutex.Unlock()
	for conn := range c.connections {
		select {
		case conn.send <- message:
		default:
			close(conn.send)
			delete(c.connections, conn)
		}
	}
}

var chatRooms = make(map[string]*ChatRoom)
var chatRoomMutex sync.Mutex

func create_room(roomID string) {
	chatRoomMutex.Lock()
	defer chatRoomMutex.Unlock()

	if _, exists := chatRooms[roomID]; !exists {
		chatRooms[roomID] = NewChatRoom(roomID)
		fmt.Printf("Room %s created\n", roomID)
	} else {
		fmt.Printf("Room %s already exists\n", roomID)
	}
}

func enter_room(roomID string, connection *Connection) {
	chatRoomMutex.Lock()
	defer chatRoomMutex.Unlock()

	room, exists := chatRooms[roomID]
	if !exists {
		fmt.Printf("Room %s does not exist\n", roomID)
		return
	}

	room.AddConnection(connection)
	fmt.Printf("Client entered room %s\n", roomID)
}

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

	// Получение room_id из параметра запроса
	roomID := r.URL.Query().Get("room_id")

	// Если параметр room_id не пустой, присоединяем клиента к комнате
	if roomID != "" {
		enter_room(roomID, connection)
	}

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

	fmt.Println("Server started on :50004")
	http.ListenAndServe(":50004", nil)
}
