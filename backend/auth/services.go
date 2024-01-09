package auth

import (
	"encoding/json"
	"net/url"

	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/log"
)

func validateGoogleUser(accessToken string) (GoogleUser, error) {
	u, _ := url.Parse("https://www.googleapis.com/oauth2/v3/tokeninfo")
	u.Query().Set("access_token", accessToken)

	request := fiber.Get(u.String())
	request.Debug()

	_, data, err := request.Bytes()
	if err != nil {
		log.Error("Coudn't get user info", err)
		return GoogleUser{}, err[0]
	}

	var googleUser GoogleUser
	jsonErr := json.Unmarshal(data, &googleUser)
	if jsonErr != nil {
		log.Error("Coudn't get user info", err)
		return GoogleUser{}, jsonErr
	}

	return googleUser, nil
}
