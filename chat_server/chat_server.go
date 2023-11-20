package main

//TODO: handle empty message

import (
	"context"
	"errors"
	"fmt"
	"log"
	"net"
	"net/http"
	"sync"

	protos "protos"

	empty "github.com/golang/protobuf/ptypes/empty"
	"github.com/google/uuid"
	"github.com/gorilla/websocket"
	"google.golang.org/grpc"
)

func generateRoomID() (string, error) {
	uuidObj, err := uuid.NewRandom()
	if err != nil {
		return "", err
	}
	return uuidObj.String(), nil
}

type gRPCServer struct {
	protos.UnimplementedChatServiceServer
}

func (s *gRPCServer) CreateRoom(ctx context.Context, req *empty.Empty) (*protos.RoomResponse, error) {
	for attempt := 0; attempt < 10; attempt++ {
		roomID, err := generateRoomID()
		if err != nil {
			return nil, err
		}

		if rooms[roomID] == nil {
			createRoom(roomID)
			return &protos.RoomResponse{RoomId: roomID}, nil
		}
	}
	return &protos.RoomResponse{Error: "Failed to generate a unique room"}, errors.New("failed to generate a unique room ID after 10 attempts")
}

// ChatRoom представляет собой комнату чата
type ChatRoom struct {
	ID          string
	clients     map[*websocket.Conn]struct{}
	clientsLock sync.Mutex
}

// ClientMessage представляет собой структуру для разбора JSON-сообщений от клиента
type ClientMessage struct {
	Text     string `json:"text"`
	Username string `json:"username"`
}

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool {
		return true
	},
}

var rooms = make(map[string]*ChatRoom)
var roomsLock sync.Mutex

func handleWebSocket(w http.ResponseWriter, r *http.Request) {
	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Println(err)
		return
	}
	defer conn.Close()

	// Получаем ID комнаты из параметра запроса
	roomID := r.URL.Query().Get("room_id")

	roomsLock.Lock()
	room, roomExists := rooms[roomID]
	if !roomExists {
		// Если комнаты не существует, закрываем соединение
		roomsLock.Unlock()
		log.Printf("Room %s does not exist. Connection closed.", roomID)
		return
	}
	roomsLock.Unlock()

	// Добавляем клиента в комнату
	room.clientsLock.Lock()
	room.clients[conn] = struct{}{}
	room.clientsLock.Unlock()

	// Обработка входящих сообщений от клиента
	for {
		var msg ClientMessage
		err := conn.ReadJSON(&msg)
		fmt.Println(msg.Username, ": ", msg.Text)
		if err != nil {
			log.Printf("Error reading message: %v", err)
			break
		}

		// Рассылаем сообщение всем клиентам в комнате
		room.clientsLock.Lock()
		for client := range room.clients {
			err := client.WriteJSON(msg)
			if err != nil {
				log.Printf("Error writing message: %v", err)
			}
		}
		room.clientsLock.Unlock()
	}

	// Удаляем клиента из комнаты при закрытии соединения
	room.clientsLock.Lock()
	delete(room.clients, conn)
	room.clientsLock.Unlock()
}

// createRoom создает новую комнату с указанным ID
func createRoom(roomID string) *ChatRoom {
	room := &ChatRoom{
		ID:      roomID,
		clients: make(map[*websocket.Conn]struct{}),
	}
	roomsLock.Lock()
	rooms[roomID] = room
	roomsLock.Unlock()
	fmt.Printf("Created room %s\n", roomID)
	return room
}

// func createRoomHandler(w http.ResponseWriter, r *http.Request) {
// 	roomID := "123"
// 	createdRoom := createRoom(roomID)
// 	fmt.Fprintf(w, "Room %s created successfully", createdRoom.ID)
// }

func main() {
	// http.HandleFunc("/create-room", createRoomHandler)

	// log.Fatal(http.ListenAndServe("0.0.0.0:50010", nil))

	// Запуск gRPC-сервера
	grpcServer := &gRPCServer{}
	go func() {
		listener, err := net.Listen("tcp", ":50041")
		if err != nil {
			log.Fatalf("Failed to listen: %v", err)
		}

		server := grpc.NewServer()
		protos.RegisterChatServiceServer(server, grpcServer)

		fmt.Println("gRPC server started on :50041")
		err = server.Serve(listener)
		if err != nil {
			log.Fatalf("Failed to serve: %v", err)
		}
	}()

	http.HandleFunc("/chat", handleWebSocket)
	fmt.Println("WebSocket server started on :50010")
	err := http.ListenAndServe(":50010", nil)
	if err != nil {
		fmt.Println("Error starting server:", err)
	}

	// createRoom("123")
	// createRoom("666")

}

// import (
// 	"context"
// 	"errors"
// 	"fmt"
// 	"log"
// 	"net"
// 	"net/http"
// 	"os"
// 	"os/signal"
// 	protos "protos"
// 	"sync"

// 	empty "github.com/golang/protobuf/ptypes/empty"
// 	"github.com/google/uuid"
// 	"github.com/gorilla/websocket"
// 	"google.golang.org/grpc"
// )

// // var chatRoomMutex sync.Mutex
// // var chatRooms = make(map[string]*ChatRoom)

// func generateRoomID() (string, error) {
// 	uuidObj, err := uuid.NewRandom()
// 	if err != nil {
// 		return "", err
// 	}

// 	return uuidObj.String(), nil
// }

// type gRPCServer struct {
// 	protos.UnimplementedChatServiceServer
// 	controller *Controller
// }

// func (s *gRPCServer) CreateRoom(ctx context.Context, req *empty.Empty) (*protos.RoomResponse, error) {
// 	for attempt := 0; attempt < 10; attempt++ {
// 		roomID, err := generateRoomID()
// 		if err != nil {
// 			return nil, err
// 		}

// 		if s.controller.chatRooms[roomID] == nil {
// 			s.controller.CreateRoom(roomID)
// 			return &protos.RoomResponse{RoomId: roomID}, nil
// 		}
// 	}

// 	return &protos.RoomResponse{Error: "Failed to generate a unique room"}, errors.New("failed to generate a unique room ID after 10 attempts")
// }

// type Connection struct {
// 	conn *websocket.Conn
// 	send chan []byte
// }

// type ChatRoom struct {
// 	ID           string
// 	connections  map[*Connection]struct{}
// 	connMutex    sync.Mutex
// 	messageQueue chan []byte
// }

// func NewChatRoom(roomID string) *ChatRoom {
// 	fmt.Printf("New chat room %s\n", roomID)
// 	return &ChatRoom{
// 		ID:           roomID,
// 		connections:  make(map[*Connection]struct{}),
// 		connMutex:    sync.Mutex{},
// 		messageQueue: make(chan []byte),
// 	}
// }

// func (c *ChatRoom) AddConnection(conn *Connection) {
// 	c.connMutex.Lock()
// 	defer c.connMutex.Unlock()
// 	c.connections[conn] = struct{}{} //?
// }

// func (c *ChatRoom) RemoveConnection(conn *Connection) {
// 	c.connMutex.Lock()
// 	fmt.Println("Connection removed")
// 	defer c.connMutex.Unlock()
// 	delete(c.connections, conn) //?
// 	close(conn.send)            //?
// }

// func (c *ChatRoom) BroadcastMessage(message []byte) {
// 	c.connMutex.Lock()
// 	defer c.connMutex.Unlock()
// 	for conn := range c.connections {
// 		select {
// 		case conn.send <- message:
// 		default:
// 			close(conn.send)
// 			delete(c.connections, conn)
// 		}
// 	}
// }

// type Controller struct {
// 	chatRooms     map[string]*ChatRoom
// 	chatRoomMutex sync.Mutex
// 	connections   map[*Connection]struct{}
// 	connMutex     sync.Mutex
// 	messageQueue  chan []byte
// 	upgrader      websocket.Upgrader
// }

// func NewController() *Controller {
// 	return &Controller{
// 		chatRooms:     make(map[string]*ChatRoom),
// 		chatRoomMutex: sync.Mutex{},
// 		connections:   make(map[*Connection]struct{}),
// 		connMutex:     sync.Mutex{},
// 		messageQueue:  make(chan []byte),
// 		upgrader: websocket.Upgrader{
// 			ReadBufferSize:  1024,
// 			WriteBufferSize: 1024,
// 		},
// 	}
// }

// func (c *Controller) CreateRoom(roomID string) {
// 	c.chatRoomMutex.Lock()
// 	defer c.chatRoomMutex.Unlock()

// 	if _, exists := c.chatRooms[roomID]; !exists {
// 		c.chatRooms[roomID] = NewChatRoom(roomID)
// 		fmt.Printf("Room %s created\n", roomID)
// 	} else {
// 		fmt.Printf("Room %s already exists\n", roomID)
// 	}
// }

// func (c *Controller) EnterRoom(roomID string, connection *Connection) {
// 	c.chatRoomMutex.Lock()
// 	defer c.chatRoomMutex.Unlock()

// 	room, exists := c.chatRooms[roomID]
// 	if !exists {
// 		fmt.Printf("Room %s does not exist\n", roomID)
// 		room.RemoveConnection(connection)
// 		// connection.conn.Close()
// 		return
// 	}

// 	room.AddConnection(connection)
// 	fmt.Printf("Client entered room %s\n", roomID)
// }

// func (c *Controller) HandleConnection(w http.ResponseWriter, r *http.Request) {
// 	conn, err := c.upgrader.Upgrade(w, r, nil)
// 	if err != nil {
// 		fmt.Println(err)
// 		return
// 	}

// 	fmt.Println("Client connected")

// 	connection := &Connection{
// 		conn: conn,
// 		send: make(chan []byte),
// 	}

// 	c.connMutex.Lock()
// 	c.connections[connection] = struct{}{}
// 	c.connMutex.Unlock()

// 	// Получение room_id из параметра запроса
// 	roomID := r.URL.Query().Get("room_id")

// 	// Если параметр room_id не пустой, присоединяем клиента к комнате
// 	if roomID != "" {
// 		c.EnterRoom(roomID, connection)
// 	}

// 	go func() {
// 		defer func() {
// 			c.connMutex.Lock()
// 			delete(c.connections, connection)
// 			c.connMutex.Unlock()
// 			close(connection.send)
// 		}()

// 		for {
// 			messageType, p, err := conn.ReadMessage()
// 			if err != nil {
// 				fmt.Println(err)
// 				return
// 			}

// 			switch messageType {
// 			case websocket.TextMessage:
// 				message := string(p)
// 				fmt.Printf("Received text message: %s\n", message)
// 				c.messageQueue <- []byte(message)
// 			case websocket.CloseMessage:
// 				// Обработка закрытия соединения
// 				c.chatRooms[roomID].RemoveConnection(connection)
// 				return
// 			case websocket.BinaryMessage:
// 				// Обработка бинарных сообщений
// 			case websocket.PingMessage:
// 				// Обработка пинг-сообщений
// 			case websocket.PongMessage:
// 				// Обработка понг-сообщений
// 			}
// 		}
// 	}()

// 	go func() {
// 		for message := range connection.send {
// 			if err := conn.WriteMessage(websocket.TextMessage, message); err != nil {
// 				fmt.Println(err)
// 				return
// 			}
// 		}
// 	}()

// 	for {
// 		select {
// 		case message := <-c.messageQueue:
// 			c.connMutex.Lock()
// 			for conn := range c.connections {
// 				select {
// 				case conn.send <- message:
// 				default:
// 					close(conn.send)
// 					delete(c.connections, conn)
// 				}
// 			}
// 			c.connMutex.Unlock()
// 		}
// 	}
// }

// func main() {
// 	controller := NewController()

// 	// Запуск gRPC-сервера
// 	grpcServer := &gRPCServer{
// 		controller: controller,
// 	}

// 	go func() {
// 		listener, err := net.Listen("tcp", ":50041")
// 		if err != nil {
// 			log.Fatalf("Failed to listen: %v", err)
// 		}

// 		server := grpc.NewServer()
// 		protos.RegisterChatServiceServer(server, grpcServer)

// 		fmt.Println("gRPC server started on :50041")
// 		err = server.Serve(listener)
// 		if err != nil {
// 			log.Fatalf("Failed to serve: %v", err)
// 		}
// 	}()

// 	// Запуск HTTP-сервера
// 	http.HandleFunc("/chat", controller.HandleConnection)
// 	http.Handle("/", http.FileServer(http.Dir("static")))

// 	// Сигнал для завершения горутин
// 	interrupt := make(chan os.Signal, 1)
// 	signal.Notify(interrupt, os.Interrupt)

// 	var wg sync.WaitGroup
// 	wg.Add(1)

// 	go func() {
// 		<-interrupt
// 		// close(controller.messageQueue)
// 		wg.Done()
// 		os.Exit(0)
// 	}()

// 	fmt.Println("WebSocket server started on :50010")
// 	err := http.ListenAndServe(":50010", nil)
// 	if err != nil {
// 		fmt.Println("Error starting server:", err)
// 	}

// 	// Ожидание завершения горутины обработки сообщений
// 	wg.Wait()
// }
