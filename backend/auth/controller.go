package auth

import (
	"encoding/json"

	"github.com/Ayrat-Kh/expenso-app/backend/helpers"
	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/log"
)

type Controller struct {
}

// Google auth login
// @Summary      Google auth login
// @Description  Google auth verification
// @Tags         auth
// @Accept       json
// @Produce      json
// @Param        data body auth.GoogleLoginRequest true "Google login request"
// @Success      200  {object} auth.UserLoginResult
// @Failure      400  {object} helpers.ErrorResponse
// @Router       /login/google-auth [post]
func (c *Controller) PostAuthGoogleUser(authService IAuthService) func(c *fiber.Ctx) error {
	return func(c *fiber.Ctx) error {
		requestId := helpers.GetRequestId(c)
		googleLoginRequest := GoogleLoginRequest{}

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
				helpers.BuildErrorResponse(helpers.CODE_GOOGLE_LOGIN_FAILED, "Couldn't do a google login"),
			)
		}

		return c.JSON(userLoginResult)
	}
}

// Get user info
// @Summary      Get user info
// @Description  Get user info
// @Tags         user
// @Param        Authorization header string true "Bearer"
// @Produce      json
// @Success      200  {object} auth.UserResult
// @Failure      400  {object} helpers.ErrorResponse
// @Router       /v1/me [get]
func (c *Controller) GetMe(authService IAuthService) func(c *fiber.Ctx) error {
	return func(c *fiber.Ctx) error {
		requestId := helpers.GetRequestId(c)

		userClaims, err := helpers.GetUserClaims(c)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_UNAUTHORIZED, err.Error()),
			)
		}

		user, err := HandleGetMe(userClaims, authService)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_GOOGLE_LOGIN_FAILED, "Invalid body"),
			)
		}

		return c.JSON(user)
	}
}
