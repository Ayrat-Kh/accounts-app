package auth_test

import (
	"testing"

	"github.com/Ayrat-Kh/expenso-app/backend/auth"
	"github.com/Ayrat-Kh/expenso-app/backend/constants"
	"github.com/Ayrat-Kh/expenso-app/backend/shared/google"
	"github.com/stretchr/testify/assert"
)

type TestAuthService struct {
	google.IGoogleApiClient
	auth.IUserRepository
}

func (t *TestAuthService) FirstOrCreateByGoogleId(gogoleId string, user auth.UserDb) (auth.UserDb, error) {
	return user, nil
}

func (t *TestAuthService) GetGoogleUser(accessToken string) (google.GoogleTokenInfo, error) {
	return google.GoogleTokenInfo{
		Sub: "SUB",
	}, nil
}

// func (t *TestAuthService) GetUser(userId guuid.UUID) (UserDb, error) {
// 	return UserDb{}, nil
// }

// TestHelloName calls greetings.Hello with a name, checking
// for a valid return value.
func TestSuccessGoogleLogin(t *testing.T) {
	token := "token"

	t.Setenv(constants.APP_JWT_SECRET, "123456")

	response, err := auth.HandleGoogleApiAuth(token, &TestAuthService{})

	assert.Nilf(t, err, `HandleGoogleApiAuth("%s", ...) shouldn't fails`, token)

	assert.NotEmptyf(t, response.AccessToken, `HandleGoogleApiAuth("%s", ...) shouldn't fails`, token)

	assert.Equalf(t, response.User.GoogleId, "SUB", `HandleGoogleApiAuth("%s", ...) GoogleId should be set`, token)
}
