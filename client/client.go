package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"

	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
)

var token string

type AuthResponse struct {
	Token string `json:"token"`
}

type Credentials struct {
	Username string `json:"username"`
	Password string `json:"password"`
}

type SendData struct {
	Recipient string `json:"recipient"`
	Message   string `json:"message"`
}

func main() {
	a := app.New()

	// Окно сообщений
	messageWindow := a.NewWindow("Messages")
	recipientEntry := widget.NewEntry()
	messageEntry := widget.NewMultiLineEntry()
	responseLabel := widget.NewLabel("") // Метка для отображения ответа сервера
	sendButton := widget.NewButton("Send", func() {
		recipient := recipientEntry.Text
		message := messageEntry.Text

		send_data := SendData{
			Recipient: recipient,
			Message:   message,
		}

		jsonData, err := json.Marshal(send_data)
		if err != nil {
			fmt.Println("Ошибка при маршалинге JSON:", err)
			return
		}

		client := &http.Client{}
		req, err := http.NewRequest("POST", "http://127.0.0.1:8080/messages", bytes.NewBuffer(jsonData))
		if err != nil {
			fmt.Println("Ошибка при создании запроса:", err)
			return
		}

		// Добавляем заголовок Authorization
		req.Header.Set("Authorization", token)

		// Выполняем запрос
		response, err := client.Do(req)
		if err != nil {
			fmt.Println("Ошибка при выполнении POST-запроса:", err)
			return
		}

		defer response.Body.Close()
		body, err := io.ReadAll(response.Body)
		if err != nil {
			fmt.Println("Ошибка при чтении тела ответа:", err)
			return
		}

		// Преобразуем тело ответа в строку и устанавливаем в виджет
		responseLabel.SetText("Ответ сервера:\n" + string(body))
	})

	messageWindow.SetContent(container.NewVBox(
		widget.NewLabel("Recipient"),
		recipientEntry,
		widget.NewLabel("Message"),
		messageEntry,
		sendButton,
		responseLabel, // Добавляем метку в контейнер
	))

	// Окно регистрации и аутентификации
	authWindow := a.NewWindow("Register/Login")
	usernameEntry := widget.NewEntry()
	passwordEntry := widget.NewPasswordEntry()
	serverResponseLabel := widget.NewLabel("") // Метка для отображения ответа сервера
	registerButton := widget.NewButton("Register", func() {
		username := usernameEntry.Text
		password := passwordEntry.Text
		credentials := Credentials{
			Username: username,
			Password: password,
		}
		jsonData, err := json.Marshal(credentials)
		if err != nil {
			fmt.Println("Ошибка при маршалинге JSON:", err)
			return
		}
		response, err := http.Post("http://127.0.0.1:8080/auth/register", "application/json", bytes.NewBuffer(jsonData))
		if err != nil {
			fmt.Println("Ошибка при выполнении POST-запроса:", err)
			return
		}
		defer response.Body.Close()
		body, err := io.ReadAll(response.Body)
		if err != nil {
			fmt.Println("Ошибка при чтении тела ответа:", err)
			return
		}

		// Отображаем ответ сервера
		serverResponseLabel.SetText(string(body))
	})

	loginButton := widget.NewButton("Login", func() {
		username := usernameEntry.Text
		password := passwordEntry.Text
		credentials := Credentials{
			Username: username,
			Password: password,
		}
		jsonData, err := json.Marshal(credentials)
		if err != nil {
			fmt.Println("Ошибка при маршалинге JSON:", err)
			return
		}
		response, err := http.Post("http://127.0.0.1:8080/auth/login", "application/json", bytes.NewBuffer(jsonData))
		if err != nil {
			fmt.Println("Ошибка при выполнении POST-запроса:", err)
			return
		}
		defer response.Body.Close()
		body, err := io.ReadAll(response.Body)
		if err != nil {
			fmt.Println("Ошибка при чтении тела ответа:", err)
			return
		}
		// Декодируем тело ответа в структуру AuthResponse
		var authResponse AuthResponse
		if err := json.Unmarshal(body, &authResponse); err != nil {
			fmt.Println("Ошибка при декодировании JSON:", err)
			return
		}

		// Извлекаем токен и устанавливаем его в глобальной переменной
		token = authResponse.Token
		// Проверяем, не пуст ли токен
		if token != "" {
			// Если вход успешен, открываем окно сообщений
			messageWindow.Show()
		} else {
			// Если токен пуст, выводим сообщение об ошибке
			serverResponseLabel.SetText("Ошибка входа: токен пуст.")
		}
	})

	authWindow.SetContent(container.NewVBox(
		widget.NewLabel("Username"),
		usernameEntry,
		widget.NewLabel("Password"),
		passwordEntry,
		container.NewHBox(
			registerButton,
			loginButton,
		),
		serverResponseLabel, // Добавляем метку в контейнер
	))

	// Показываем окно регистрации и аутентификации
	authWindow.Show()
	a.Run()
}
