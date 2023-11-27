FROM golang:1.21-alpine AS builder

WORKDIR /usr/local/src

COPY ["go.mod", "go.sum", "./"]

RUN go mod download

COPY . ./
RUN go build -o ./bin/app chat_server.go

FROM alpine

COPY --from=builder /usr/local/src/bin/app /

CMD [ "./app" ]