package auth

import user "github.com/Ayrat-Kh/expenso-app/backend/user"

// service dtos
type GoogleLoginRequest struct {
	AccessToken string `json:"accessToken"`
}

type UserLoginResult struct {
	user.UserResult
	AccessToken  string `json:"accessToken"`
	SessionToken string `json:"sessionToken"`
}
