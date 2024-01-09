package auth

type GoogleTokenInfo struct {
	Azp           string `json:"azp"`
	Aud           string `json:"aud"`
	Sub           string `json:"sub"`
	Scope         string `json:"scope"`
	Exp           string `json:"exp"`        // num
	ExpiresIn     string `json:"expires_in"` // num
	Email         string `json:"email"`
	EmailVerified string `json:"email_verified"` // bool
	AccessType    string `json:"access_type"`
}

type GoogleLoginRequest struct {
	AccessToken string `json:"accessToken" xml:"accessToken" form:"accessToken"`
}
