package auth

import (
	"github.com/gofiber/fiber/v2"
)

func Initalize(app *fiber.App) {
	app.Post("/login/google-auth", authGoogleUser)
}
