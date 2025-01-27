package repository

import (
	"context"
	"errors"
	"github.com/Go-CT-ITMO/library-daerys/internal/domain/library"
	"sync"
)

var (
	BookNotFoundError      = errors.New("book not found")
	BookAlreadyExistsError = errors.New("book already exists")
)

type BookRepository interface {
	AddBook(ctx context.Context, book library.Book) error
	UpdateBook(ctx context.Context, book library.Book) error
	GetBookInfo(ctx context.Context, ID string) (*library.Book, error)
	GetAuthorBooks(ctx context.Context, authorID string) ([]library.Book, error)
}

type bookRepository struct {
	data map[string]*library.Book
	mux  *sync.RWMutex
}

func NewBookRepository() *bookRepository {
	return &bookRepository{
		data: make(map[string]*library.Book),
		mux:  new(sync.RWMutex),
	}
}

func (repo *bookRepository) AddBook(ctx context.Context, book library.Book) error {
	repo.mux.Lock()
	defer repo.mux.Unlock()
	if _, exists := repo.data[book.ID]; exists {
		return BookAlreadyExistsError
	}
	repo.data[book.ID] = &book
	return nil
}

func (repo *bookRepository) UpdateBook(ctx context.Context, book library.Book) error {
	repo.mux.Lock()
	defer repo.mux.Unlock()
	if repo.data[book.ID] == nil {
		return BookNotFoundError
	}
	repo.data[book.ID].Name = book.Name
	return nil
}

func (repo *bookRepository) GetBookInfo(ctx context.Context, ID string) (*library.Book, error) {
	repo.mux.Lock()
	defer repo.mux.Unlock()
	if book := repo.data[ID]; book != nil {
		return book, nil
	}
	return nil, BookNotFoundError
}

func (repo *bookRepository) GetAuthorBooks(ctx context.Context, authorID string) ([]library.Book, error) {
	repo.mux.RLock()
	defer repo.mux.RUnlock()

	var result []library.Book
	for _, book := range repo.data {
		for _, id := range book.AuthorIDs {
			if id == authorID {
				result = append(result, *book)
				break
			}
		}
	}

	if len(result) == 0 {
		return nil, errors.New("no books found for author")
	}

	return result, nil
}
