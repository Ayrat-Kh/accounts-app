package auth

import (
	"github.com/gofiber/fiber/v3"
)

func Initalize(app *fiber.App) {
	app.Post("/login/google-auth", Controller.handleAuthGoogleUser(AuthService))
}
