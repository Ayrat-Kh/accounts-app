package auth

type GoogleUser struct {
	Id string
}

type GoogleLoginRequest struct {
	accessToken string
}
