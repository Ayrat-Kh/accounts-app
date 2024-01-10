package auth

import (
	"encoding/json"

	"github.com/Ayrat-Kh/expenso-app/backend/helpers"
	"github.com/Ayrat-Kh/expenso-app/backend/shared/google"
	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/log"
)

type controller struct {
}

var Controller controller = controller{}

func (c *controller) handleAuthGoogleUser(authService IAuthService) func(c *fiber.Ctx) error {
	return func(c *fiber.Ctx) error {
		requestId := helpers.GetRequestId(c)
		googleLoginRequest := google.GoogleLoginRequest{}

		if err := json.Unmarshal(c.Body(), &googleLoginRequest); err != nil {
			log.Errorf("%s %s %s", requestId, helpers.CODE_INVALID_BODY, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_INVALID_BODY, "Invalid body"),
			)
		}

		userLoginResult, err := handleGoogleApiAuth(googleLoginRequest.AccessToken, authService)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_GOOGLE_LOGIN_FAILED, "Invalid body"),
			)
		}

		return c.JSON(userLoginResult)
	}
}
