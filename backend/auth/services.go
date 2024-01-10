package auth

import (
	"os"
	"time"

	"github.com/Ayrat-Kh/expenso-app/backend/constants"
	"github.com/Ayrat-Kh/expenso-app/backend/shared/google"

	"github.com/golang-jwt/jwt"
)

type IAuthService interface {
	google.IGoogleApiClient
	IUserRepository
}

type AuthService struct {
	google.GoogleApiClient
	UserRepository
}

func mapUserDbToUserDto(userDb UserDb) UserDto {
	return UserDto{
		CreatedAt: userDb.CreatedAt,
		GoogleId:  userDb.GoogleId,
		Username:  userDb.Username,
		Email:     userDb.Email,
	}
}

func createUserClaims(userDto UserDto) jwt.MapClaims {
	return jwt.MapClaims{
		"id":  userDto.Id,
		"exp": time.Now().Add(time.Hour * 24 * 7).Unix(),
	}
}

func createUserAccessToken(userDto UserDto) (string, error) {
	// Create token
	token := jwt.NewWithClaims(jwt.SigningMethodHS256, createUserClaims(userDto))

	// Generate encoded token and send it as response.
	t, err := token.SignedString([]byte(os.Getenv(constants.APP_JWT_SECRET)))

	return t, err
}

func HandleGoogleApiAuth(accessToken string, authService IAuthService) (UserLoginResult, error) {
	result := UserLoginResult{}
	user, err := authService.GetGoogleUser(accessToken)

	if err != nil {
		return result, err
	}

	userDb, err := authService.FirstOrCreateByGoogleId(user.Sub, UserDb{GoogleId: user.Sub, Email: user.Email})

	if err != nil {
		return result, err
	}

	result.User = mapUserDbToUserDto(userDb)

	// Generate encoded token and send it as response.
	result.AccessToken, err = createUserAccessToken(result.User)

	return result, err
}
