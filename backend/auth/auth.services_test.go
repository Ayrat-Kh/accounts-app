package auth_test

import (
	"context"
	"testing"

	"github.com/Ayrat-Kh/expenso-app/backend/auth"
	"github.com/Ayrat-Kh/expenso-app/backend/constants"
	"github.com/stretchr/testify/assert"
)

func TestSuccessGoogleLogin(t *testing.T) {
	token := "token"

	t.Setenv(constants.APP_JWT_SECRET, "123456")

	response, err := auth.MockAuthService.GoogleAuth(token, context.Background())

	assert.Nilf(t, err, `HandleGoogleApiAuth("%s", ...) shouldn't fails`, token)

	assert.NotEmptyf(t, response.AccessToken, `HandleGoogleApiAuth("%s", ...) shouldn't fails`, token)

	assert.Equalf(t, response.User.GoogleId, "SUB1", `HandleGoogleApiAuth("%s", ...) GoogleId should be set`, token)
}
