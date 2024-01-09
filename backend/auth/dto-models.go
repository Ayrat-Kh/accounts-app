package auth

type GoogleTokenInfo struct {
	azp            string
	aud            string
	sub            string
	scope          string
	exp            string // num
	expires_in     string // num
	email          string
	email_verified string // bool
	access_type    string
}

type GoogleLoginRequest struct {
	AccessToken string `json:"accessToken" xml:"accessToken" form:"accessToken"`
}
