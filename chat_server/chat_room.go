// chat_room.go
package main

// type ChatRoom struct {
// 	ID           string
// 	connections  map[*Connection]struct{}
// 	connMutex    sync.Mutex
// 	messageQueue chan []byte
// }

// func NewChatRoom(roomID string) *ChatRoom {
// 	return &ChatRoom{
// 		ID:           roomID,
// 		connections:  make(map[*Connection]struct{}),
// 		messageQueue: make(chan []byte),
// 	}
// }

// func (c *ChatRoom) AddConnection(conn *Connection) {
// 	c.connMutex.Lock()
// 	defer c.connMutex.Unlock()
// 	c.connections[conn] = struct{}{}
// }

// func (c *ChatRoom) RemoveConnection(conn *Connection) {
// 	c.connMutex.Lock()
// 	defer c.connMutex.Unlock()
// 	delete(c.connections, conn)
// 	close(conn.send)
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
