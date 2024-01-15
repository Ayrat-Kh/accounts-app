package auth

import (
	"context"
	"os"
	"time"

	"github.com/Ayrat-Kh/expenso-app/backend/constants"
	"github.com/Ayrat-Kh/expenso-app/backend/helpers"
	"github.com/Ayrat-Kh/expenso-app/backend/shared/google"
	userPackage "github.com/Ayrat-Kh/expenso-app/backend/user"
	"github.com/golang-jwt/jwt/v5"
	guuid "github.com/google/uuid"
)

func createUserClaims(userId string) jwt.MapClaims {
	return jwt.MapClaims{
		constants.APP_JWT_USER_ID: userId,
		constants.APP_JWT_EXP:     time.Now().Add(time.Hour * 24 * 7).Unix(),
	}
}

func createUserAccessToken(userId guuid.UUID) (string, error) {
	// Create token
	token := jwt.NewWithClaims(jwt.SigningMethodHS256, createUserClaims(userId.String()))

	// Generate encoded token and send it as response.
	t, err := token.SignedString([]byte(os.Getenv(constants.APP_JWT_SECRET)))

	return t, err
}

type AuthServiceInterface interface {
	GoogleAuth(accessToken string, context context.Context) (UserLoginResult, error)
}

type authService struct {
	google.GoogleLoginInterface
	userPackage.UserServiceInterface
	userSessionRepositoryInterface
}

func (self *authService) GoogleAuth(accessToken string, context context.Context) (UserLoginResult, error) {
	result := UserLoginResult{}
	user, err := self.GetGoogleUser(accessToken)

	if err != nil {
		return result, err
	}

	userDb, err := self.UserServiceInterface.UpsertUserByGoogleId(user.Sub, userPackage.UserDb{GoogleId: user.Sub, Email: user.Email})

	if err != nil {
		return result, err
	}

	result.User = userPackage.MapUserDbToUserDto(userDb)

	// Generate encoded token and send it as response.
	result.AccessToken, err = createUserAccessToken(result.User.Id)
	result.SessionToken = helpers.RandStringRunes(32)

	err = self.userSessionRepositoryInterface.InvalidateSessionForUser(userDb.Id)
	if err != nil {
		return result, err
	}

	_, err = self.userSessionRepositoryInterface.CreateSession(SessionDb{
		UserId:       userDb.Id,
		IsValid:      true,
		AccessToken:  result.AccessToken,
		SessionToken: result.SessionToken,
	})
	if err != nil {
		return result, err
	}

	return result, err
}

var AuthService AuthServiceInterface = &authService{
	UserServiceInterface:           userPackage.UserService,
	userSessionRepositoryInterface: &userSessionRepository{},
	GoogleLoginInterface:           &google.GoogleApiClient{},
}
