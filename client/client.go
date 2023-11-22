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
var messages_q []WsInclomingMessage

func main() {
	a := app.New()

	// Main window
	mainWindow := a.NewWindow("Chat App")
	size := fyne.Size{
		Width:  600.0,
		Height: 500.0,
	}
	mainWindow.Resize(size)
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

		// Convert response body to a string and set it in the widget
		// responseLabel.SetText("Server response:\n" + string(body))
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

	// Окно для чата
	chatMessages := widget.NewLabel("")
	chatEntry := widget.NewEntry()
	chatEntry.SetPlaceHolder("Enter your message...")
	// chatLabel := widget.NewLabel("")
	chatSendButton := widget.NewButton("Send", func() {
		// Логика отправки сообщения через WebSocket
		message := chatEntry.Text
		if message != "" {
			sendWsMessage(conn, message)
			chatEntry.SetText("") // Очистить поле ввода после отправки
		}
	})

	// Создаем окно для чата
	chatWindow := container.NewVBox(
		chatMessages, // Виджет для отображения сообщений
		// chatLabel,
		chatEntry,
		chatSendButton,
	)

	go func() {
		for {
			// Проверяем наличие новых сообщений
			if len(messages_q) > 0 {
				// Обновляем виджет с сообщениями
				newText := chatMessages.Text + "\n" + messages_q[0].Username + ": " + messages_q[0].Text
				chatMessages.SetText(newText)

				// Удаляем обработанное сообщение из очереди
				messages_q = messages_q[1:]
			}

			// Добавьте небольшую задержку, чтобы не перегружать CPU
			time.Sleep(100 * time.Millisecond)
		}
	}()
	userListLabel := widget.NewLabel("")
	userList := []string{}
	userEntry := widget.NewEntry()
	userEntry.SetPlaceHolder("Enter name of user to invite...")

	userEntry.OnSubmitted = func(text string) {
		if text != "" {
			userList = append(userList, text)
			userListLabel.SetText(strings.Join(userList, " | "))
			userEntry.SetText("")
		}
	}

	newChatButton := widget.NewButton("New chat", func() {
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

		go readWsMessage(conn, &messages_q)

		mainWindow.SetContent(chatWindow)

	})

	roomIdEntry := widget.NewEntry() // форма для ввода рум айди для подключения к чату
	roomIdEntry.SetPlaceHolder("Enter room ID...")

	connectToChatButton := widget.NewButton("Connect", func() {

	})

	chatMenuWindow := container.NewVBox(
		widget.NewLabelWithStyle("Start New Chat", fyne.TextAlignCenter, fyne.TextStyle{Bold: true}),
		userListLabel,
		userEntry,
		newChatButton,
		widget.NewLabel(""),
		widget.NewLabelWithStyle("Connect To Chat", fyne.TextAlignCenter, fyne.TextStyle{Bold: true}),
		roomIdEntry,
		connectToChatButton,
	)

	messageWindow := container.NewVBox(
		widget.NewLabel("Recipient"),
		recipientEntry,
		widget.NewLabel("Message"),
		messageEntry,
		sendButton,
		responseLabel, // Widget to display server response
	)

	// Window for retrieving messages
	getMessagesLabel := widget.NewLabel("")
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
			messageText := "Incoming messages:\n"
			for _, msg := range messages {
				messageText += fmt.Sprintf("%s: %s\n", msg.Sender, msg.Text)
			}
			getMessagesLabel.SetText(messageText)
		} else {
			getMessagesLabel.SetText("No incoming messages.")
		}
	})

	getMessagesWindow := container.NewVBox(
		getMessagesButton,
		getMessagesLabel,
	)

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
				container.NewTabItem("Send Messages", messageWindow),
				container.NewTabItem("Get Messages", getMessagesWindow),
				container.NewTabItem("Chat", chatMenuWindow),
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
		container.NewHBox(
			registerButton,
			loginButton,
		),
		serverResponseLabel, // Widget to display server response
	)

	// Create tabs
	tabs := container.NewAppTabs(
		// container.NewTabItem("Messages", messageWindow),
		// container.NewTabItem("Get Messages", getMessagesWindow),
		// container.NewTabItem("Chat", chatMenuWindow),
		container.NewTabItem("Auth", authWindow),
	)

	// Set tabs in the main window
	mainWindow.SetContent(tabs)

	// Show only the authentication window on launch
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
