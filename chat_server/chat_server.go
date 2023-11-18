package main

//TODO: fix rooms

import (
	"context"
	"errors"
	"fmt"
	"log"
	"net"
	"net/http"
	"os"
	"os/signal"
	protos "protos"
	"sync"

	empty "github.com/golang/protobuf/ptypes/empty"
	"github.com/google/uuid"
	"github.com/gorilla/websocket"
	"google.golang.org/grpc"
)

// var chatRoomMutex sync.Mutex
// var chatRooms = make(map[string]*ChatRoom)

func generateRoomID() (string, error) {
	uuidObj, err := uuid.NewRandom()
	if err != nil {
		return "", err
	}

	return uuidObj.String(), nil
}

type gRPCServer struct {
	protos.UnimplementedChatServiceServer
	controller *Controller
}

func (s *gRPCServer) CreateRoom(ctx context.Context, req *empty.Empty) (*protos.RoomResponse, error) {
	for attempt := 0; attempt < 10; attempt++ {
		roomID, err := generateRoomID()
		if err != nil {
			return nil, err
		}

		if s.controller.chatRooms[roomID] == nil {
			s.controller.CreateRoom(roomID)
			return &protos.RoomResponse{RoomId: roomID}, nil
		}
	}

	return &protos.RoomResponse{Error: "Failed to generate a unique room"}, errors.New("failed to generate a unique room ID after 10 attempts")
}

type Connection struct {
	conn *websocket.Conn
	send chan []byte
}

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
	fmt.Println("Connection removed")
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

type Controller struct {
	chatRooms     map[string]*ChatRoom
	chatRoomMutex sync.Mutex
	connections   map[*Connection]struct{}
	connMutex     sync.Mutex
	messageQueue  chan []byte
	upgrader      websocket.Upgrader
}

func NewController() *Controller {
	return &Controller{
		chatRooms:     make(map[string]*ChatRoom),
		chatRoomMutex: sync.Mutex{},
		connections:   make(map[*Connection]struct{}),
		connMutex:     sync.Mutex{},
		messageQueue:  make(chan []byte),
		upgrader: websocket.Upgrader{
			ReadBufferSize:  1024,
			WriteBufferSize: 1024,
		},
	}
}

func (c *Controller) CreateRoom(roomID string) {
	c.chatRoomMutex.Lock()
	defer c.chatRoomMutex.Unlock()

	if _, exists := c.chatRooms[roomID]; !exists {
		c.chatRooms[roomID] = NewChatRoom(roomID)
		fmt.Printf("Room %s created\n", roomID)
	} else {
		fmt.Printf("Room %s already exists\n", roomID)
	}
}

func (c *Controller) EnterRoom(roomID string, connection *Connection) {
	c.chatRoomMutex.Lock()
	defer c.chatRoomMutex.Unlock()

	room, exists := c.chatRooms[roomID]
	if !exists {
		fmt.Printf("Room %s does not exist\n", roomID)
		return
	}

	room.AddConnection(connection)
	fmt.Printf("Client entered room %s\n", roomID)
}

func (c *Controller) HandleConnection(w http.ResponseWriter, r *http.Request) {
	conn, err := c.upgrader.Upgrade(w, r, nil)
	if err != nil {
		fmt.Println(err)
		return
	}

	fmt.Println("Client connected")

	connection := &Connection{
		conn: conn,
		send: make(chan []byte),
	}

	c.connMutex.Lock()
	c.connections[connection] = struct{}{}
	c.connMutex.Unlock()

	// Получение room_id из параметра запроса
	roomID := r.URL.Query().Get("room_id")

	// Если параметр room_id не пустой, присоединяем клиента к комнате
	if roomID != "" {
		c.EnterRoom(roomID, connection)
	}

	go func() {
		defer func() {
			c.connMutex.Lock()
			delete(c.connections, connection)
			c.connMutex.Unlock()
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
				c.messageQueue <- []byte(message)
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
		case message := <-c.messageQueue:
			c.connMutex.Lock()
			for conn := range c.connections {
				select {
				case conn.send <- message:
				default:
					close(conn.send)
					delete(c.connections, conn)
				}
			}
			c.connMutex.Unlock()
		}
	}
}

func main() {
	controller := NewController()

	// Запуск gRPC-сервера
	grpcServer := &gRPCServer{
		controller: controller,
	}

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

	// Запуск HTTP-сервера
	http.HandleFunc("/chat", controller.HandleConnection)
	http.Handle("/", http.FileServer(http.Dir("static")))

	// Сигнал для завершения горутин
	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt)

	var wg sync.WaitGroup
	wg.Add(1)

	go func() {
		<-interrupt
		close(controller.messageQueue)
		wg.Done()
	}()

	fmt.Println("WebSocket server started on :50010")
	err := http.ListenAndServe(":50010", nil)
	if err != nil {
		fmt.Println("Error starting server:", err)
	}

	// Ожидание завершения горутины обработки сообщений
	wg.Wait()
}
