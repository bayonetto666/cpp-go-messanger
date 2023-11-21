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
}
