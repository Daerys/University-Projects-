package usecases

import "github.com/google/uuid"

func isValidUUID(id string) bool {
	_, err := uuid.Parse(id)
	return err == nil
}
