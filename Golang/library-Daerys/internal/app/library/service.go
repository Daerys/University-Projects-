package library

import (
	"context"
	"github.com/Go-CT-ITMO/library-daerys/internal/app/library/repository"
	"github.com/Go-CT-ITMO/library-daerys/internal/pkg/usecases"
	desc "github.com/Go-CT-ITMO/library-daerys/pkg/generated/proto/api"
	"google.golang.org/grpc"
)

type Implementation struct {
	BookUseCase   usecases.BookUseCase
	AuthorUseCase usecases.AuthorUseCase
	desc.UnimplementedLibraryServer
}

func NewImplementation() *Implementation {
	return &Implementation{
		BookUseCase:   usecases.NewBookUseCase(repository.NewBookRepository()),
		AuthorUseCase: usecases.NewAuthorUseCase(repository.NewAuthorRepository()),
	}
}

func (s *Implementation) AddBook(ctx context.Context, req *desc.AddBookRequest) (*desc.AddBookResponse, error) {
	return s.BookUseCase.AddBook(ctx, req)
}

func (s *Implementation) UpdateBook(ctx context.Context, req *desc.UpdateBookRequest) (*desc.UpdateBookResponse, error) {
	return s.BookUseCase.UpdateBook(ctx, req)
}

func (s *Implementation) GetBookInfo(ctx context.Context, req *desc.GetBookInfoRequest) (*desc.GetBookInfoResponse, error) {
	return s.BookUseCase.GetBookInfo(ctx, req)
}

func (s *Implementation) GetAuthorBooks(req *desc.GetAuthorBooksRequest, stream grpc.ServerStreamingServer[desc.Book]) error {
	return s.BookUseCase.GetAuthorBooks(req, stream)
}

func (s *Implementation) RegisterAuthor(ctx context.Context, req *desc.RegisterAuthorRequest) (*desc.RegisterAuthorResponse, error) {
	return s.AuthorUseCase.RegisterAuthor(ctx, req)
}

func (s *Implementation) ChangeAuthorInfo(ctx context.Context, req *desc.ChangeAuthorInfoRequest) (*desc.ChangeAuthorInfoResponse, error) {
	return s.AuthorUseCase.ChangeAuthorInfo(ctx, req)
}

func (s *Implementation) GetAuthorInfo(ctx context.Context, req *desc.GetAuthorInfoRequest) (*desc.GetAuthorInfoResponse, error) {
	return s.AuthorUseCase.GetAuthorInfo(ctx, req)
}
