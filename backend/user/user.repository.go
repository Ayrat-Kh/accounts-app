package user

import (
	"database/sql"

	"github.com/Ayrat-Kh/expenso-app/backend/database"
	guuid "github.com/google/uuid"
)

type userRepositoryInterface interface {
	GetUserById(userId guuid.UUID) (UserDb, error)
	UpdateUser(userId guuid.UUID, user UpdateUserDb) (UserDb, error)
	UpsertUserByGoogleId(googleId string, user UserDb) (UserDb, error)
}

type userRepository struct {
}

func (rep *userRepository) GetUserById(userId guuid.UUID) (UserDb, error) {
	user := UserDb{}

	database.DB.Model(&UserDb{}).First(&user, userId)

	return user, nil
}

func (rep *userRepository) UpdateUser(userId guuid.UUID, user UpdateUserDb) (UserDb, error) {
	updateUser := UserDb{}

	result := database.DB.Model(&updateUser).Where("id = @userId", sql.Named("userId", userId)).Updates(&user)

	return updateUser, result.Error
}

func (rep *userRepository) UpsertUserByGoogleId(googleId string, user UserDb) (UserDb, error) {
	result := database.DB.Model(&user).Where("google_id = @googleId", sql.Named("googleId", googleId)).Save(&user)

	return user, result.Error
}
