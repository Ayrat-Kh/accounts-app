package auth_test

import (
	"testing"
	"time"

	"github.com/Ayrat-Kh/expenso-app/backend/auth"
	"github.com/Ayrat-Kh/expenso-app/backend/constants"
	"github.com/Ayrat-Kh/expenso-app/backend/shared/google"
	"github.com/golang-jwt/jwt/v5"
	guuid "github.com/google/uuid"
	"github.com/stretchr/testify/assert"
)

type TestAuthService struct{}

func (t *TestAuthService) FirstOrCreateByGoogleId(gogoleId string, user auth.UserDb) (auth.UserDb, error) {
	user.GoogleId = user.GoogleId + "1"
	return user, nil
}

func (t *TestAuthService) GetGoogleUser(accessToken string) (google.GoogleTokenInfo, error) {
	return google.GoogleTokenInfo{
		Sub: "SUB",
	}, nil
}

func (t *TestAuthService) GetUserById(userId guuid.UUID) (auth.UserDb, error) {
	return auth.UserDb{
		GoogleId: "googleId",
		Username: "username",
	}, nil
}

func TestSuccessGoogleLogin(t *testing.T) {
	token := "token"

	t.Setenv(constants.APP_JWT_SECRET, "123456")

	response, err := auth.HandleGoogleApiAuth(token, &TestAuthService{})

	assert.Nilf(t, err, `HandleGoogleApiAuth("%s", ...) shouldn't fails`, token)

	assert.NotEmptyf(t, response.AccessToken, `HandleGoogleApiAuth("%s", ...) shouldn't fails`, token)

	assert.Equalf(t, response.User.GoogleId, "SUB1", `HandleGoogleApiAuth("%s", ...) GoogleId should be set`, token)
}

func TestSuccessGetMe(t *testing.T) {
	token := "token"

	t.Setenv(constants.APP_JWT_SECRET, "123456")

	response, err := auth.HandleGetMe(jwt.MapClaims{
		constants.APP_JWT_USER_ID: "123456",
		constants.APP_JWT_EXP:     time.Now().Add(time.Hour * 24 * 7).Unix(),
	}, &TestAuthService{})

	assert.Nilf(t, err, `HandleGoogleApiAuth("%s", ...) shouldn't fails`, token)

	assert.Equal(t, response.User.Username, "username", `HandleGoogleApiAuth("%s", ...) shouldn't fails`, token)
}
