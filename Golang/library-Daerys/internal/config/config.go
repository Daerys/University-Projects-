package config

const grpcHost = "localhost"

const grpcPort = "9090"

const httpPort = "8080"

func MustGetGrpcAddr() string {
	return ":" + grpcPort
}

func MustGetGrpcURL() string {
	return grpcHost + ":" + grpcPort
}

func MustGetHttpAddr() string {
	return ":" + httpPort
}
