package google

import (
	"encoding/json"
	"errors"
	"net/http"
	"net/url"
)

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

type IGoogleApiClient interface {
	GetGoogleUser(accessToken string) (GoogleTokenInfo, error)
}

type GoogleApiClientImpl struct {
	IGoogleApiClient
}

func (client *GoogleApiClientImpl) GetGoogleUser(accessToken string) (GoogleTokenInfo, error) {
	result := GoogleTokenInfo{}

	u, _ := url.Parse("https://www.googleapis.com/oauth2/v3/tokeninfo")
	queryValues := u.Query()
	queryValues.Set("access_token", accessToken)
	u.RawQuery = queryValues.Encode()

	response, err := http.Get(u.String())

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
