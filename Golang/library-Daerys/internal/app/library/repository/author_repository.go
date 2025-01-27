package repository

import (
	"context"
	"errors"
	"github.com/Go-CT-ITMO/library-daerys/internal/domain/library"
	"sync"
)

var (
	AuthorNotFoundError      = errors.New("author not found")
	AuthorAlreadyExistsError = errors.New("author already exists")
)

type AuthorRepository interface {
	RegisterAuthor(ctx context.Context, author library.Author) error
	UpdateAuthor(ctx context.Context, author library.Author) error
	GetAuthorByID(ctx context.Context, ID string) (*library.Author, error)
}

type authorRepository struct {
	data map[string]*library.Author
	mux  *sync.RWMutex
}

func NewAuthorRepository() *authorRepository {
	return &authorRepository{
		data: make(map[string]*library.Author),
		mux:  new(sync.RWMutex),
	}
}

func (repo *authorRepository) RegisterAuthor(ctx context.Context, author library.Author) error {
	repo.mux.Lock()
	defer repo.mux.Unlock()
	if repo.data[author.ID] != nil {
		return AuthorAlreadyExistsError
	}
	repo.data[author.ID] = &author
	return nil
}

func (repo *authorRepository) UpdateAuthor(ctx context.Context, author library.Author) error {
	repo.mux.Lock()
	defer repo.mux.Unlock()
	if repo.data[author.ID] == nil {
		return AuthorNotFoundError
	}
	repo.data[author.ID].Name = author.Name
	return nil
}

func (repo *authorRepository) GetAuthorByID(ctx context.Context, ID string) (*library.Author, error) {
	repo.mux.Lock()
	defer repo.mux.Unlock()
	if author := repo.data[ID]; author != nil {
		return author, nil
	}
	return nil, AuthorNotFoundError
}
