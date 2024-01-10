package auth

import (
	"github.com/Ayrat-Kh/expenso-app/backend/shared/google"
)

type IAuthService interface {
	google.IGoogleApiClient
	IUserRepository
}

type authServiceImpl struct {
	IAuthService
}

var AuthService authServiceImpl

func handleGoogleApiAuth(
	accessToken string,
	requestId string,
	deps IAuthService) (UserDto, error) {
	result := UserDto{}
	user, err := deps.GetGoogleUser(accessToken, requestId)

	if err != nil {
		return result, err
	}

	userDb, err := deps.FirstOrCreateByGoogleId(user.Sub, UserDb{GoogleId: user.Sub, Email: user.Email})

	result.CreatedAt = userDb.CreatedAt
	result.GoogleId = userDb.GoogleId
	result.Username = userDb.Username
	result.Email = userDb.Email

	return result, err
}
