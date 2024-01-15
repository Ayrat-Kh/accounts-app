package auth

import (
	"database/sql"

	"github.com/Ayrat-Kh/expenso-app/backend/database"
	guuid "github.com/google/uuid"
)

type userSessionRepositoryInterface interface {
	CreateSession(session SessionDb) (SessionDb, error)
	InvalidateSessionForUser(userId guuid.UUID) error
}

type userSessionRepository struct{}

func (rep *userSessionRepository) CreateSession(session SessionDb) (SessionDb, error) {
	result := database.DB.Model(&session).Create(&session)
	return session, result.Error
}

func (rep *userSessionRepository) InvalidateSessionForUser(userId guuid.UUID) error {
	session := SessionDb{}
	result := database.DB.Model(&session).Where("user_id = @userId", sql.Named("userId", userId)).Updates(&SessionDb{
		IsValid: false,
	})
	return result.Error
}
