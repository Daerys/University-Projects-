package usecases

import (
	"context"
	"errors"
	"github.com/Go-CT-ITMO/library-daerys/internal/app/library/repository"
	"github.com/Go-CT-ITMO/library-daerys/internal/domain/library"
	desc "github.com/Go-CT-ITMO/library-daerys/pkg/generated/proto/api"
	"github.com/google/uuid"
	"google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
)

var (
	BookUseCaseIternalError   = errors.New("use case: internal server error")
	BookUseCaseInvalidIdError = errors.New("use case: invalid book ID format")
)

type BookUseCase interface {
	AddBook(context.Context, *desc.AddBookRequest) (*desc.AddBookResponse, error)
	UpdateBook(context.Context, *desc.UpdateBookRequest) (*desc.UpdateBookResponse, error)
	GetBookInfo(context.Context, *desc.GetBookInfoRequest) (*desc.GetBookInfoResponse, error)
	GetAuthorBooks(*desc.GetAuthorBooksRequest, grpc.ServerStreamingServer[desc.Book]) error
}

type bookUseCase struct {
	repo repository.BookRepository
}

func NewBookUseCase(repo repository.BookRepository) *bookUseCase {
	return &bookUseCase{repo: repo}
}

func (useCase *bookUseCase) AddBook(ctx context.Context, req *desc.AddBookRequest) (*desc.AddBookResponse, error) {
	bookID, err := uuid.NewRandom()

	if err != nil {
		return nil, status.Error(codes.Internal, BookUseCaseIternalError.Error())
	}

	book := library.Book{
		ID:        bookID.String(),
		Name:      req.GetName(),
		AuthorIDs: req.GetAuthorId(),
	}
	if err := useCase.repo.AddBook(ctx, book); err != nil {
		if errors.Is(err, repository.BookAlreadyExistsError) {
			return nil, status.Error(codes.AlreadyExists, err.Error())
		}
		return nil, status.Error(codes.Internal, BookUseCaseIternalError.Error())
	}
	return &desc.AddBookResponse{
		Book: &desc.Book{
			Id:       bookID.String(),
			Name:     book.Name,
			AuthorId: book.AuthorIDs,
		},
	}, nil
}

func (useCase *bookUseCase) UpdateBook(ctx context.Context, req *desc.UpdateBookRequest) (*desc.UpdateBookResponse, error) {

	if !isValidUUID(req.GetId()) {
		return nil, status.Error(codes.InvalidArgument, BookUseCaseInvalidIdError.Error())
	}

	book := library.Book{
		ID:   req.GetId(),
		Name: req.GetName(),
	}
	if err := useCase.repo.UpdateBook(ctx, book); err != nil {
		if errors.Is(err, repository.BookNotFoundError) {
			return nil, status.Error(codes.NotFound, err.Error())
		}
		return nil, status.Error(codes.Internal, BookUseCaseIternalError.Error())
	}
	return &desc.UpdateBookResponse{}, nil
}

func (useCase *bookUseCase) GetBookInfo(ctx context.Context, req *desc.GetBookInfoRequest) (*desc.GetBookInfoResponse, error) {

	if !isValidUUID(req.GetId()) {
		return nil, status.Error(codes.InvalidArgument, BookUseCaseInvalidIdError.Error())
	}

	book, err := useCase.repo.GetBookInfo(ctx, req.Id)
	if err != nil {
		if errors.Is(err, repository.BookNotFoundError) {
			return nil, status.Error(codes.NotFound, err.Error())
		}
		return nil, status.Error(codes.Internal, BookUseCaseIternalError.Error())
	}
	return &desc.GetBookInfoResponse{Book: &desc.Book{
		Id:       book.ID,
		Name:     book.Name,
		AuthorId: book.AuthorIDs,
	}}, nil
}

func (useCase *bookUseCase) GetAuthorBooks(req *desc.GetAuthorBooksRequest, stream grpc.ServerStreamingServer[desc.Book]) error {

	if !isValidUUID(req.GetAuthorId()) {
		return status.Error(codes.InvalidArgument, AuthorUseCaseInvalidIdError.Error())
	}

	books, err := useCase.repo.GetAuthorBooks(context.Background(), req.GetAuthorId())
	if err != nil {
		if errors.Is(err, repository.AuthorNotFoundError) {
			return status.Error(codes.NotFound, "author not found")
		}
		return status.Error(codes.Internal, "failed to get author books")
	}

	for _, book := range books {
		pbBook := &desc.Book{
			Id:       book.ID,
			Name:     book.Name,
			AuthorId: book.AuthorIDs,
		}

		if err := stream.Send(pbBook); err != nil {
			return status.Error(codes.Internal, "failed to send book stream")
		}
	}

	return nil
}
