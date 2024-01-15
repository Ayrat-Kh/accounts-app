package user_test

import (
	"context"
	"testing"

	"github.com/Ayrat-Kh/expenso-app/backend/helpers"
	"github.com/Ayrat-Kh/expenso-app/backend/user"
	guuid "github.com/google/uuid"
	"github.com/stretchr/testify/assert"
)

func TestSuccessGetUser(t *testing.T) {
	response, err := user.MockUserService.GetUser(
		guuid.UUID{},

		context.Background(),
	)

	assert.Nil(t, err, `TestSuccessGetUser shouldn't fail`)

	assert.Equal(t, response.User.FirstName, "username", `TestSuccessGetUser should get FirstName`)
}

func TestSuccessUpdateUser(t *testing.T) {
	response, err := user.MockUserService.UpdateUser(
		guuid.UUID{},
		user.UpdateUserDto{
			FirstName: "firstname",
			LastName:  "lastname",
		},
		helpers.BuildAuthContextWithValue(context.Background(), guuid.UUID{}),
	)

	assert.Nilf(t, err, `HandleGoogleApiAuth shouldn't fail`)

	assert.Equal(t, response.User.FirstName, "firstnameupdated", `UpdateUser should update FirstName`)
	assert.Equal(t, response.User.LastName, "lastnameupdated", `UpdateUser should update LastName`)
	assert.Equal(t, response.User.Alias, "updated", `UpdateUser should update Alias`)
}
