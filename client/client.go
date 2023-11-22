package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
)

var token string

type AuthResponse struct {
	Token string `json:"token"`
}

type RegResponse struct {
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
		responseLabel.SetText("Server response:\n" + string(body))
	})

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
				container.NewTabItem("Messages", messageWindow),
				container.NewTabItem("Get Messages", getMessagesWindow),
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
		container.NewTabItem("Messages", messageWindow),
		container.NewTabItem("Get Messages", getMessagesWindow),
		container.NewTabItem("Auth", authWindow),
	)

	// Set tabs in the main window
	mainWindow.SetContent(tabs)

	// Show only the authentication window on launch
	mainWindow.SetContent(authWindow)

	mainWindow.ShowAndRun()
}
