package auth

import (
	"github.com/Ayrat-Kh/expenso-app/backend/database"
	guuid "github.com/google/uuid"
)

type IUserRepository interface {
	GetUser(userId guuid.UUID) (UserDb, error)
	SaveUser(user UserDb) (UserDb, error)
	FirstOrCreate(userId guuid.UUID, user UserDb) (UserDb, error)
	FirstOrCreateByGoogleId(googleId string, user UserDb) (UserDb, error)
}

type userRepositoryImpl struct{}

func (rep *userRepositoryImpl) GetUser(userId guuid.UUID) (UserDb, error) {
	user := UserDb{}

	database.DB.First(&user, userId)

	return user, nil
}

func (rep *userRepositoryImpl) SaveUser(user UserDb) (UserDb, error) {
	result := database.DB.Create(&user)

	return user, result.Error
}

func (rep *userRepositoryImpl) FirstOrCreate(userId guuid.UUID, user UserDb) (UserDb, error) {
	result := database.DB.FirstOrCreate(&user, UserDb{Id: userId})

	return user, result.Error
}

func (rep *userRepositoryImpl) FirstOrCreateByGoogleId(googleId string, user UserDb) (UserDb, error) {
	result := database.DB.FirstOrCreate(&user, UserDb{GoogleId: googleId})

	return user, result.Error
}

var UserRepository userRepositoryImpl
