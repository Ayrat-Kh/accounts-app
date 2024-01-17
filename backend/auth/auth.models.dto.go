package auth

import user "github.com/Ayrat-Kh/expenso-app/backend/user"

// service dtos
type GoogleLoginRequest struct {
	AccessToken string `json:"accessToken" validate:"required"`
}

type UserLoginResult struct {
	user.UserResult
	AccessToken  string `json:"accessToken" validate:"required"`
	SessionToken string `json:"sessionToken" validate:"required"`
}
