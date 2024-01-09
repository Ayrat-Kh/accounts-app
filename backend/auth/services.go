package auth

import (
	"encoding/json"
	"net/url"

	"github.com/gofiber/fiber/v3"
	"github.com/gofiber/fiber/v3/log"
)

func validateGoogleUser(accessToken string, requestId string) (GoogleTokenInfo, error) {
	result := GoogleTokenInfo{}

	u, _ := url.Parse("https://www.googleapis.com/oauth2/v3/tokeninfo")
	queryValues := u.Query()
	queryValues.Set("access_token", accessToken)
	u.RawQuery = queryValues.Encode()

	request := fiber.Get(u.String())

	_, data, err := request.Bytes()

	if err != nil {
		log.Errorf("%s Coudn't get user info %s", requestId, err)
		return result, err[0]
	}

	if err := json.Unmarshal(data, &result); err != nil {
		log.Errorf("%s Coudn't parse user info %s", requestId, err)
		return GoogleTokenInfo{}, err
	}

	return result, nil
}
