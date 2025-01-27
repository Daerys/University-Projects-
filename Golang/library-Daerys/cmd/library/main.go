package main

import (
	"context"
	"github.com/Go-CT-ITMO/library-daerys/internal/config"
	"log"
	"net"
	"net/http"

	"github.com/Go-CT-ITMO/library-daerys/internal/app/library"
	decs "github.com/Go-CT-ITMO/library-daerys/pkg/generated/proto/api" // Исправьте импорт
	"github.com/grpc-ecosystem/grpc-gateway/v2/runtime"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
)

func runRest(ctx context.Context) {
	ctx, cancel := context.WithCancel(ctx)
	defer cancel()
	mux := runtime.NewServeMux()
	opts := []grpc.DialOption{grpc.WithTransportCredentials(insecure.NewCredentials())}
	err := decs.RegisterLibraryHandlerFromEndpoint(ctx, mux, config.MustGetGrpcURL(), opts)
	if err != nil {
		panic(err)
	}
	log.Printf("server listening at 8081")
	if err = http.ListenAndServe(config.MustGetHttpAddr(), mux); err != nil {
		panic(err)
	}
}

func runGrpc(noteService *library.Implementation) {
	lis, err := net.Listen("tcp", config.MustGetGrpcAddr())
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}
	s := grpc.NewServer()
	decs.RegisterLibraryServer(s, noteService)
	log.Printf("server listening at %v", lis.Addr())
	if err = s.Serve(lis); err != nil {
		panic(err)
	}
}

func main() {
	ctx := context.Background()
	noteService := library.NewImplementation()

	go runRest(ctx)
	runGrpc(noteService)
}
