package auth

import (
	"encoding/json"
	"net/url"

	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/log"
)

func validateGoogleUser(accessToken string) (GoogleTokenInfo, error) {
	u, _ := url.Parse("https://www.googleapis.com/oauth2/v3/tokeninfo")
	queryValues := u.Query()
	u.RawQuery = queryValues.Encode()

	request := fiber.Get(u.String())

	_, data, err := request.Bytes()
	if err != nil {
		log.Error("Coudn't get user info", err)
		return GoogleTokenInfo{}, err[0]
	}

	var googleUser GoogleTokenInfo
	if jsonErr := json.Unmarshal(data, &googleUser); jsonErr != nil {
		log.Error("Coudn't parse user info", err)
		return GoogleTokenInfo{}, jsonErr
	}

	return googleUser, nil
}
