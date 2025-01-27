package usecases

import (
	"context"
	"errors"
	"github.com/Go-CT-ITMO/library-daerys/internal/app/library/repository"
	"github.com/Go-CT-ITMO/library-daerys/internal/domain/library"
	desc "github.com/Go-CT-ITMO/library-daerys/pkg/generated/proto/api"
	"github.com/google/uuid"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
)

var (
	AuthorUseCaseIternalError   = errors.New("use case: internal server error")
	AuthorUseCaseInvalidIdError = errors.New("use case: invalid author ID format")
)

type AuthorUseCase interface {
	RegisterAuthor(context.Context, *desc.RegisterAuthorRequest) (*desc.RegisterAuthorResponse, error)
	ChangeAuthorInfo(context.Context, *desc.ChangeAuthorInfoRequest) (*desc.ChangeAuthorInfoResponse, error)
	GetAuthorInfo(context.Context, *desc.GetAuthorInfoRequest) (*desc.GetAuthorInfoResponse, error)
}

type authorUseCase struct {
	repo repository.AuthorRepository
}

func NewAuthorUseCase(repo repository.AuthorRepository) *authorUseCase {
	return &authorUseCase{repo: repo}
}

func (useCase *authorUseCase) RegisterAuthor(ctx context.Context, req *desc.RegisterAuthorRequest) (*desc.RegisterAuthorResponse, error) {
	authorID, err := uuid.NewRandom()

	if err != nil {
		return nil, status.Error(codes.Internal, BookUseCaseIternalError.Error())
	}
	author := library.Author{
		ID:   authorID.String(),
		Name: req.GetName(),
	}

	if err := useCase.repo.RegisterAuthor(ctx, author); err != nil {
		if errors.Is(err, repository.AuthorAlreadyExistsError) {
			return nil, status.Error(codes.AlreadyExists, err.Error())
		}
		return nil, status.Error(codes.Internal, AuthorUseCaseIternalError.Error())
	}
	return &desc.RegisterAuthorResponse{
		Id: authorID.String(),
	}, nil
}

func (useCase *authorUseCase) ChangeAuthorInfo(ctx context.Context, req *desc.ChangeAuthorInfoRequest) (*desc.ChangeAuthorInfoResponse, error) {

	if !isValidUUID(req.GetId()) {
		return nil, status.Error(codes.InvalidArgument, AuthorUseCaseInvalidIdError.Error())
	}

	author := library.Author{
		ID:   req.GetId(),
		Name: req.GetName(),
	}
	if err := useCase.repo.UpdateAuthor(ctx, author); err != nil {
		if errors.Is(err, repository.AuthorNotFoundError) {
			return nil, status.Error(codes.NotFound, err.Error())
		}
		return nil, status.Error(codes.Internal, AuthorUseCaseIternalError.Error())
	}
	return &desc.ChangeAuthorInfoResponse{}, nil
}

func (useCase *authorUseCase) GetAuthorInfo(ctx context.Context, req *desc.GetAuthorInfoRequest) (*desc.GetAuthorInfoResponse, error) {

	if !isValidUUID(req.GetId()) {
		return nil, status.Error(codes.InvalidArgument, AuthorUseCaseInvalidIdError.Error())
	}

	author, err := useCase.repo.GetAuthorByID(ctx, req.GetId())
	if err != nil {
		if errors.Is(err, repository.AuthorNotFoundError) {
			return nil, status.Error(codes.NotFound, err.Error())
		}
		return nil, status.Error(codes.Internal, AuthorUseCaseIternalError.Error())
	}
	return &desc.GetAuthorInfoResponse{
		Id:   author.ID,
		Name: author.Name,
	}, nil
}
