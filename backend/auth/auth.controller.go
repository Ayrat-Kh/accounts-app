package auth

import (
	"context"
	"encoding/json"

	"github.com/Ayrat-Kh/expenso-app/backend/helpers"
	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/log"
)

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
func postAuthGoogleUser(app *fiber.App) {
	app.Post("/login/google-auth", func(c *fiber.Ctx) error {
		ctx := helpers.BuildAppContext(c, context.Background())
		requestId := helpers.GetCtxRequestId(ctx)

		googleLoginRequest := GoogleLoginRequest{}

		if err := json.Unmarshal(c.Body(), &googleLoginRequest); err != nil {
			log.Errorf("%s %s %s", requestId, helpers.CODE_INVALID_BODY, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_INVALID_BODY, "Invalid body"),
			)
		}

		userLoginResult, err := AuthService.GoogleAuth(googleLoginRequest.AccessToken, ctx)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_GOOGLE_LOGIN_FAILED, "Couldn't do a google login"),
			)
		}

		return c.JSON(userLoginResult)
	})
}

func InitPublic(app *fiber.App) {
	postAuthGoogleUser(app)
}
