package auth

import (
	"github.com/Ayrat-Kh/expenso-app/backend/database"
	guuid "github.com/google/uuid"
)

type IUserRepository interface {
	GetUserById(userId guuid.UUID) (UserDb, error)
	FirstOrCreateByGoogleId(googleId string, user UserDb) (UserDb, error)
}

type UserRepository struct{}

func (rep *UserRepository) GetUserById(userId guuid.UUID) (UserDb, error) {
	user := UserDb{}

	database.DB.First(&user, userId)

	return user, nil
}

func (rep *UserRepository) SaveUser(user UserDb) (UserDb, error) {
	result := database.DB.Create(&user)

	return user, result.Error
}

func (rep *UserRepository) FirstOrCreate(userId guuid.UUID, user UserDb) (UserDb, error) {
	result := database.DB.FirstOrCreate(&user, UserDb{Id: userId})

	return user, result.Error
}

func (rep *UserRepository) FirstOrCreateByGoogleId(googleId string, user UserDb) (UserDb, error) {
	result := database.DB.Where(&UserDb{GoogleId: googleId}).FirstOrCreate(&user)

	return user, result.Error
}
