package auth_test

import (
	"testing"

	"github.com/Ayrat-Kh/expenso-app/backend/auth"
	"github.com/Ayrat-Kh/expenso-app/backend/constants"
	"github.com/Ayrat-Kh/expenso-app/backend/shared/google"
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

	if err != nil {
		t.Fatalf(`HandleGoogleApiAuth("%s", ...) shouldn't fails`, token)
	}

	if response.AccessToken == "" {
		t.Fatalf(`HandleGoogleApiAuth("%s", ...) AccessToken should be set`, token)
	}

	if response.User.GoogleId != "SUB" {
		t.Fatalf(`HandleGoogleApiAuth("%s", ...) GoogleId should be set`, token)
	}
}
