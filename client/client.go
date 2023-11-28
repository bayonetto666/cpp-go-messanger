package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"net/url"
	"strings"

	"time"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"

	"github.com/gorilla/websocket"
)

type WsMessage struct {
	Text string `json:"text"`
}

type WsInclomingMessage struct {
	Text     string `json:"text"`
	Username string `json:"username"`
}

type AuthResponse struct {
	Token string `json:"token"`
}

type RegResponse struct {
	Success string `json:"success"`
	Error   string `json:"error"`
}

type SendResponse struct {
	Success string `json:"success"`
	Error   string `json:"error"`
}

type Credentials struct {
	Username string `json:"username"`
	Password string `json:"password"`
}

type SendData struct {
	Recipient string `json:"recipient"`
	Message   string `json:"message"`
}

type Message struct {
	Sender string `json:"sender"`
	Text   string `json:"text"`
}

var conn *websocket.Conn
var token string
var messagesQ []WsInclomingMessage

func main() {
	a := app.New()

	mainWindow := a.NewWindow("Chat App")
	size := fyne.Size{
		Width:  600.0,
		Height: 300.0,
	}
	mainWindow.Resize(size)

	authWindow := NewAuthWindow(mainWindow)

	tabs := container.NewAppTabs(
		container.NewTabItem("Auth", authWindow),
	)

	mainWindow.SetContent(tabs)
	mainWindow.SetContent(authWindow)

	mainWindow.ShowAndRun()
}

func sendWsMessage(c *websocket.Conn, message string) {
	data := WsMessage{
		Text: message,
	}

	jsonData, err := json.Marshal(data)
	if err != nil {
		log.Fatal("Ошибка преобразования в JSON:", err)
	}

	err = c.WriteMessage(websocket.TextMessage, jsonData)
	if err != nil {
		log.Println("Ошибка отправки:", err)
		return
	}

}

func readWsMessage(c *websocket.Conn, messages *[]WsInclomingMessage) {
	for {
		data := WsInclomingMessage{}
		err := c.ReadJSON(&data)
		if err != nil {
			log.Fatal("Ошибка чтения JSON:", err)
		}
		*messages = append(*messages, data)
	}

}

func NewChatWindow() fyne.CanvasObject {
	chatMessages := widget.NewMultiLineEntry()
	chatEntry := widget.NewEntry()
	chatEntry.SetPlaceHolder("Enter your message...")
	chatSendButton := widget.NewButton("Send", func() {
		message := chatEntry.Text
		if message != "" {
			sendWsMessage(conn, message)
			chatEntry.SetText("")
		}
	})

	chatWindow := container.NewVBox(
		chatMessages,
		chatEntry,
		chatSendButton,
	)

	go func() {
		for {
			if len(messagesQ) > 0 {
				newText := chatMessages.Text + "\n" + messagesQ[0].Username + ": " + messagesQ[0].Text
				chatMessages.SetText(newText)

				messagesQ = messagesQ[1:]
			}

			time.Sleep(100 * time.Millisecond)
		}
	}()
	return chatWindow
}

func NewChatMenuWindow(mainWindow fyne.Window) fyne.CanvasObject {
	userListLabel := widget.NewLabel("")
	userList := []string{}
	errorLabel := widget.NewLabel("")
	userEntry := widget.NewEntry()
	userEntry.SetPlaceHolder("Enter name of user to invite(press Enter)")

	userEntry.OnSubmitted = func(text string) {
		if text != "" {
			userList = append(userList, text)
			userListLabel.SetText(strings.Join(userList, " | "))
			userEntry.SetText("")
		}
	}

	newChatButton := widget.NewButton("New chat", func() {
		if len(userList) == 0 {
			errorLabel.SetText("No users invited")
			return
		}

		u := url.URL{Scheme: "ws", Host: "localhost:8080", Path: "/chat/new"}
		headers := http.Header{}
		headers.Add("Authorization", token)
		headers.Add("invited_users", strings.Join(userList, ","))

		dialer := websocket.DefaultDialer
		var err error
		conn, _, err = dialer.Dial(u.String(), headers)
		if err != nil {
			log.Fatal("Ошибка подключения:", err)
		}
		// defer c.Close()

		go readWsMessage(conn, &messagesQ)

		mainWindow.SetContent(NewChatWindow())
	})

	roomIdEntry := widget.NewEntry()
	roomIdEntry.SetPlaceHolder("Enter room ID...")

	connectToChatButton := widget.NewButton("Connect", func() {
		room_id := roomIdEntry.Text
		u := url.URL{Scheme: "ws", Host: "localhost:8080", Path: "/chat/connect"}
		headers := http.Header{}
		headers.Add("Authorization", token)
		headers.Add("room_id", room_id)

		dialer := websocket.DefaultDialer
		var err error
		conn, _, err = dialer.Dial(u.String(), headers)
		if err != nil {
			log.Fatal("Ошибка подключения:", err)
		}
		// defer c.Close()

		go readWsMessage(conn, &messagesQ)

		mainWindow.SetContent(NewChatWindow())
	})

	chatMenuWindow := container.NewVBox(
		widget.NewLabelWithStyle("Start New Chat", fyne.TextAlignCenter, fyne.TextStyle{Bold: true}),
		userListLabel,
		userEntry,
		newChatButton,
		errorLabel,
		widget.NewLabelWithStyle("Connect To Chat", fyne.TextAlignCenter, fyne.TextStyle{Bold: true}),
		roomIdEntry,
		connectToChatButton,
	)
	return chatMenuWindow
}

func NewSendMessageWindow() fyne.CanvasObject {
	// Message window
	recipientEntry := widget.NewEntry()
	messageEntry := widget.NewMultiLineEntry()
	responseLabel := widget.NewLabel("")
	sendButton := widget.NewButton("Send", func() {
		// Logic for sending a message
		recipient := recipientEntry.Text
		message := messageEntry.Text

		sendData := SendData{
			Recipient: recipient,
			Message:   message,
		}

		jsonData, err := json.Marshal(sendData)
		if err != nil {
			fmt.Println("JSON marshaling error:", err)
			return
		}

		client := &http.Client{}
		req, err := http.NewRequest("POST", "http://127.0.0.1:8080/messages", bytes.NewBuffer(jsonData))
		if err != nil {
			fmt.Println("Error creating request:", err)
			return
		}

		req.Header.Set("Authorization", token)

		response, err := client.Do(req)
		if err != nil {
			fmt.Println("Error executing POST request:", err)
			return
		}

		defer response.Body.Close()
		body, err := io.ReadAll(response.Body)
		if err != nil {
			fmt.Println("Error reading response body:", err)
			return
		}

		var sendResponse SendResponse
		if err := json.Unmarshal(body, &sendResponse); err != nil {
			fmt.Println("JSON unmarshaling error:", err)
			return
		}
		if sendResponse.Error != "" {
			responseLabel.SetText(sendResponse.Error)
		} else {
			responseLabel.SetText(sendResponse.Success)
		}
	})
	messageWindow := container.NewVBox(
		widget.NewLabel("Recipient"),
		recipientEntry,
		widget.NewLabel("Message"),
		messageEntry,
		sendButton,
		responseLabel, // Widget to display server response
	)
	return messageWindow
}

func NewGetMessageWindow() fyne.CanvasObject {
	// Window for retrieving messages
	getMessagesText := widget.NewMultiLineEntry()
	getMessagesText.Wrapping = fyne.TextWrapWord

	getMessagesButton := widget.NewButton("Get Messages", func() {
		// Logic for retrieving messages
		client := &http.Client{}
		req, err := http.NewRequest("GET", "http://127.0.0.1:8080/messages", nil)
		if err != nil {
			fmt.Println("Error creating request:", err)
			return
		}

		req.Header.Set("Authorization", token)

		response, err := client.Do(req)
		if err != nil {
			fmt.Println("Error executing GET request:", err)
			return
		}

		defer response.Body.Close()
		decoder := json.NewDecoder(response.Body)

		var messages []Message
		for {
			var msg Message
			if err := decoder.Decode(&msg); err == io.EOF {
				break
			} else if err != nil {
				fmt.Println("Error decoding JSON:", err)
				return
			}

			messages = append(messages, msg)
		}

		// Display messages or a message indicating their absence
		if len(messages) > 0 {
			var messageText strings.Builder
			for _, msg := range messages {
				messageText.WriteString(fmt.Sprintf("%s: %s\n", msg.Sender, msg.Text))
			}
			getMessagesText.SetText(messageText.String())
		} else {
			getMessagesText.SetText("No incoming messages.")
		}
	})

	getMessagesWindow := container.NewVBox(
		getMessagesButton,
		getMessagesText,
	)
	return getMessagesWindow
}

func NewAuthWindow(mainWindow fyne.Window) fyne.CanvasObject {
	// Window for registration and authentication
	usernameEntry := widget.NewEntry()
	passwordEntry := widget.NewPasswordEntry()
	serverResponseLabel := widget.NewLabel("") // Widget to display server response
	registerButton := widget.NewButton("Register", func() {
		// Logic for registration
		username := usernameEntry.Text
		password := passwordEntry.Text

		// Check that fields are not empty
		if username == "" || password == "" {
			serverResponseLabel.SetText("Enter a username and password.")
			return
		}
		credentials := Credentials{
			Username: username,
			Password: password,
		}
		jsonData, err := json.Marshal(credentials)
		if err != nil {
			fmt.Println("JSON marshaling error:", err)
			return
		}
		response, err := http.Post("http://127.0.0.1:8080/auth/register", "application/json", bytes.NewBuffer(jsonData))
		if err != nil {
			fmt.Println("Error executing POST request:", err)
			return
		}
		defer response.Body.Close()
		body, err := io.ReadAll(response.Body)
		if err != nil {
			fmt.Println("Error reading response body:", err)
			return
		}
		// Decode response body into AuthResponse structure
		// var authResponse AuthResponse
		var registerResponse RegResponse
		if err := json.Unmarshal(body, &registerResponse); err != nil {
			fmt.Println("JSON unmarshaling error:", err)
			return
		}
		if registerResponse.Error != "" {
			serverResponseLabel.SetText(registerResponse.Error)
		} else {
			serverResponseLabel.SetText(registerResponse.Success)
		}
	})
	loginButton := widget.NewButton("Login", func() {
		username := usernameEntry.Text
		password := passwordEntry.Text

		// Check that fields are not empty
		if username == "" || password == "" {
			serverResponseLabel.SetText("Enter a username and password.")
			return
		}
		credentials := Credentials{
			Username: username,
			Password: password,
		}
		jsonData, err := json.Marshal(credentials)
		if err != nil {
			fmt.Println("JSON marshaling error:", err)
			return
		}
		response, err := http.Post("http://127.0.0.1:8080/auth/login", "application/json", bytes.NewBuffer(jsonData))
		if err != nil {
			fmt.Println("Error executing POST request:", err)
			return
		}
		defer response.Body.Close()
		body, err := io.ReadAll(response.Body)
		if err != nil {
			fmt.Println("Error reading response body:", err)
			return
		}
		// Decode response body into AuthResponse structure
		var authResponse AuthResponse
		if err := json.Unmarshal(body, &authResponse); err != nil {
			fmt.Println("JSON unmarshaling error:", err)
			return
		}

		token = authResponse.Token
		if token != "" {
			// If login is successful, switch the content to the message window
			mainWindow.SetContent(container.NewAppTabs(
				container.NewTabItem("Send Messages", NewSendMessageWindow()),
				container.NewTabItem("Get Messages", NewGetMessageWindow()),
				container.NewTabItem("Chat", NewChatMenuWindow(mainWindow)),
			))
		} else {
			// If the token is empty, display an error message in the registration and authentication window
			serverResponseLabel.SetText("Login error.")
		}
	})

	authWindow := container.NewVBox(
		widget.NewLabel("Username"),
		usernameEntry,
		widget.NewLabel("Password"),
		passwordEntry,
		registerButton,
		loginButton,
		serverResponseLabel,
	)
	return authWindow
}
