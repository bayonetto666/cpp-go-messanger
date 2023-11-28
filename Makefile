.PHONY: generate
generate:
	protoc --cpp_out=./common --grpc_out=./common --plugin=protoc-gen-grpc --proto_path=./protos AuthServer.proto
	protoc --cpp_out=./common --grpc_out=./common --plugin=protoc-gen-grpc --proto_path=./protos DatabaseHandler.proto
