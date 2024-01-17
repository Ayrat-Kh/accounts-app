package google

import (
	"encoding/json"
	"errors"
	"net/http"
	"net/url"
)

type GoogleTokenInfo struct {
	Iss           string `json:"iss"`
	Azp           string `json:"azp"`
	Aud           string `json:"aud"`
	Sub           string `json:"sub"`
	Exp           string `json:"exp"` // num
	Email         string `json:"email"`
	EmailVerified string `json:"email_verified"` // bool
	AtHash        string `json:"at_hash"`
	GivenName     string `json:"given_name"`
	FamilyName    string `json:"family_name"`
	Name          string `json:"name"`
	Picture       string `json:"picture"`
	Locale        string `json:"locale"`
	Iat           string `json:"iat"`
	ALg           string `json:"alg"`
	Kid           string `json:"kid"`
	Typ           string `json:"typ"`
}

type GoogleLoginInterface interface {
	GetGoogleUser(idToken string) (GoogleTokenInfo, error)
}

type GoogleApiClient struct {
	GoogleLoginInterface
}

func (client *GoogleApiClient) GetGoogleUser(idToken string) (GoogleTokenInfo, error) {
	result := GoogleTokenInfo{}

	u, _ := url.Parse("https://www.googleapis.com/oauth2/v3/tokeninfo")
	queryValues := u.Query()
	queryValues.Set("id_token", idToken)
	u.RawQuery = queryValues.Encode()

	response, err := http.Post(u.String(), "application/x-www-form-urlencoded", nil)

	if err != nil {
		return GoogleTokenInfo{}, err
	}

	if response.StatusCode > http.StatusIMUsed {
		return GoogleTokenInfo{}, errors.New("Bad request")
	}

	defer response.Body.Close()

	if err = json.NewDecoder(response.Body).Decode(&result); err != nil {
		return GoogleTokenInfo{}, err
	}

	return result, nil
}
