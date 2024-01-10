package auth

import (
	"github.com/gofiber/fiber/v2"
)

func Initalize(app *fiber.App) {
	constroller := Controller{}

	app.Post("/login/google-auth", constroller.postAuthGoogleUser(&AuthService{}))
	app.Post("/v1/me", constroller.getMe(&AuthService{}))
}
