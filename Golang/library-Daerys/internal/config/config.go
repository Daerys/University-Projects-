package config

import (
	"os"
)

const (
	defaultGrpcHost    = "localhost"
	defaultGrpcPort    = "9090"
	defaultHttpPort    = "8080"
	envGrpcPort        = "GRPC_PORT"
	envGrpcGatewayPort = "GRPC_GATEWAY_PORT"
)

func MustGetGrpcAddr() string {
	port := os.Getenv(envGrpcPort)
	if port == "" {
		port = defaultGrpcPort
	}
	return ":" + port
}

func MustGetGrpcURL() string {
	port := os.Getenv(envGrpcPort)
	if port == "" {
		port = defaultGrpcPort
	}
	return defaultGrpcHost + ":" + port
}

func MustGetHttpAddr() string {
	port := os.Getenv(envGrpcGatewayPort)
	if port == "" {
		port = defaultHttpPort
	}
	return ":" + port
}
