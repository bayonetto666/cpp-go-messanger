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

type Message struct {
	Sender string `json:"sender"`
	Text   string `json:"text"`
}

func main() {
	a := app.New()

	// Основное окно
	mainWindow := a.NewWindow("Chat App")

	// Окно сообщений
	recipientEntry := widget.NewEntry()
	messageEntry := widget.NewMultiLineEntry()
	responseLabel := widget.NewLabel("") // Метка для отображения ответа сервера
	sendButton := widget.NewButton("Send", func() {
		// Логика отправки сообщения
		recipient := recipientEntry.Text
		message := messageEntry.Text

		sendData := SendData{
			Recipient: recipient,
			Message:   message,
		}

		jsonData, err := json.Marshal(sendData)
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

	messageWindow := container.NewVBox(
		widget.NewLabel("Recipient"),
		recipientEntry,
		widget.NewLabel("Message"),
		messageEntry,
		sendButton,
		responseLabel, // Метка для отображения ответа сервера
	)

	// Окно для получения сообщений
	getMessagesLabel := widget.NewLabel("")
	getMessagesButton := widget.NewButton("Get Messages", func() {
		// Логика получения сообщений
		client := &http.Client{}
		req, err := http.NewRequest("GET", "http://127.0.0.1:8080/messages", nil)
		if err != nil {
			fmt.Println("Ошибка при создании запроса:", err)
			return
		}

		// Добавляем заголовок Authorization
		req.Header.Set("Authorization", token)

		// Выполняем запрос
		response, err := client.Do(req)
		if err != nil {
			fmt.Println("Ошибка при выполнении GET-запроса:", err)
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
				fmt.Println("Ошибка при декодировании JSON:", err)
				return
			}

			messages = append(messages, msg)
		}

		// Отображаем сообщения или сообщение об их отсутствии
		if len(messages) > 0 {
			messageText := "Входящие сообщения:\n"
			for _, msg := range messages {
				messageText += fmt.Sprintf("%s: %s\n", msg.Sender, msg.Text)
			}
			getMessagesLabel.SetText(messageText)
		} else {
			getMessagesLabel.SetText("Входящих сообщений нет.")
		}
	})

	getMessagesWindow := container.NewVBox(
		getMessagesButton,
		getMessagesLabel,
	)

	// Окно регистрации и аутентификации
	usernameEntry := widget.NewEntry()
	passwordEntry := widget.NewPasswordEntry()
	serverResponseLabel := widget.NewLabel("") // Метка для отображения ответа сервера
	registerButton := widget.NewButton("Register", func() {
		// Логика регистрации
	})
	loginButton := widget.NewButton("Login", func() {
		username := usernameEntry.Text
		password := passwordEntry.Text

		// Проверяем, что поля не пустые
		if username == "" || password == "" {
			serverResponseLabel.SetText("Введите имя пользователя и пароль.")
			return
		}
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
			// Если вход успешен, заменяем содержимое окна аутентификации на окно сообщений
			mainWindow.SetContent(container.NewAppTabs(
				container.NewTabItem("Messages", messageWindow),
				container.NewTabItem("Get Messages", getMessagesWindow),
			))
		} else {
			// Если токен пуст, выводим сообщение об ошибке в окне регистрации и аутентификации
			serverResponseLabel.SetText("Ошибка входа: токен пуст.")
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
		serverResponseLabel, // Метка для отображения ответа сервера
	)

	// Создаем вкладки
	tabs := container.NewAppTabs(
		container.NewTabItem("Messages", messageWindow),
		container.NewTabItem("Get Messages", getMessagesWindow),
		container.NewTabItem("Auth", authWindow),
	)

	// Устанавливаем вкладки в основное окно
	mainWindow.SetContent(tabs)

	// Показываем только окно аутентификации при запуске
	mainWindow.SetContent(authWindow)

	mainWindow.ShowAndRun()
}
