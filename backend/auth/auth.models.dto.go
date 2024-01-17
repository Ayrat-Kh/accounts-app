package auth

import user "github.com/Ayrat-Kh/expenso-app/backend/user"

// service dtos
type GoogleLoginRequest struct {
	IdToken string `json:"idToken" validate:"required"`
}

type UserLoginResult struct {
	user.UserResult
	AccessToken  string `json:"accessToken" validate:"required"`
	SessionToken string `json:"sessionToken" validate:"required"`
}
