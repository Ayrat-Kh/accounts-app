package auth

import (
	"encoding/json"

	"github.com/Ayrat-Kh/expenso-app/backend/helpers"
	"github.com/Ayrat-Kh/expenso-app/backend/shared/google"
	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/log"
	"github.com/golang-jwt/jwt/v5"
)

type Controller struct {
}

func (c *Controller) postAuthGoogleUser(authService IAuthService) func(c *fiber.Ctx) error {
	return func(c *fiber.Ctx) error {
		requestId := helpers.GetRequestId(c)
		googleLoginRequest := google.GoogleLoginRequest{}

		if err := json.Unmarshal(c.Body(), &googleLoginRequest); err != nil {
			log.Errorf("%s %s %s", requestId, helpers.CODE_INVALID_BODY, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_INVALID_BODY, "Invalid body"),
			)
		}

		userLoginResult, err := HandleGoogleApiAuth(googleLoginRequest.AccessToken, authService)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_GOOGLE_LOGIN_FAILED, "Invalid body"),
			)
		}

		return c.JSON(userLoginResult)
	}
}

func (c *Controller) getMe(authService IAuthService) func(c *fiber.Ctx) error {
	return func(c *fiber.Ctx) error {
		requestId := helpers.GetRequestId(c)

		user, err := HandleGetMe(c.Locals("user").(*jwt.Token).Claims, authService)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_GOOGLE_LOGIN_FAILED, "Invalid body"),
			)
		}

		return c.JSON(user)
	}
}
