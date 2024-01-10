package router

import (
	"log"
	"os"

	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/middleware/logger"
	"github.com/gofiber/fiber/v2/middleware/requestid"

	"github.com/Ayrat-Kh/expenso-app/backend/auth"
	"github.com/Ayrat-Kh/expenso-app/backend/constants"
	"github.com/Ayrat-Kh/expenso-app/backend/helpers"

	jwtware "github.com/gofiber/contrib/jwt"
)

func Initalize(app *fiber.App) {
	jwtSecret := os.Getenv(constants.APP_JWT_SECRET)
	if jwtSecret == "" {
		log.Fatalln("Please set " + constants.APP_JWT_SECRET + "env")
	}

	app.Use(requestid.New())

	app.Use(logger.New(logger.Config{
		Format: "${pid} ${locals:requestid} ${status} - ${method} ${path}​\n",
	}))

	app.Get("/", func(c *fiber.Ctx) error {
		return c.Status(200).SendString("Hello, World!")
	})

	auth.Initalize(app)

	app.Use(jwtware.New(jwtware.Config{
		SigningKey: jwtware.SigningKey{Key: []byte(jwtSecret)},
	}))

	app.Use(func(c *fiber.Ctx) error {
		return c.Status(404).JSON(helpers.BuildErrorResponse(helpers.CODE_NOT_FOUND, "NotFound"))
	})

}
