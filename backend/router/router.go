package router

import (
	"log"
	"os"

	"github.com/gofiber/fiber/v2"

	"github.com/Ayrat-Kh/expenso-app/backend/auth"
	"github.com/Ayrat-Kh/expenso-app/backend/constants"
)

func Initalize(app *fiber.App) {

	// router.Use(middleware.Security)

	jwtSecret := os.Getenv(constants.APP_JWT_SECRET)

	if jwtSecret == "" {
		log.Fatalln("Please set " + constants.APP_JWT_SECRET + "env")
	}

	// app.Use(jwtware.New(jwtware.Config{
	// 	SigningKey: jwtware.SigningKey{Key: []byte(jwtSecret)},
	// }))

	app.Get("/", func(c *fiber.Ctx) error {
		return c.Status(200).SendString("Hello, World!")
	})

	auth.Initalize(app)

	// router.Use(middleware.Json)

	// users := router.Group("/users")
	// users.Post("/", handlers.CreateUser)
	// users.Delete("/", middleware.Authenticated, handlers.DeleteUser)
	// users.Put("/", middleware.Authenticated, handlers.ChangePassword)
	// users.Post("/me", middleware.Authenticated, handlers.GetUserInfo)
	// users.Post("/login", handlers.Login)
	// users.Delete("/logout", handlers.Logout)

	// products := router.Group("/products", middleware.Authenticated)
	// products.Post("/", handlers.CreateProduct)
	// products.Post("/all", handlers.GetProducts)
	// products.Delete("/:id", handlers.DeleteProduct)
	// products.Post("/:id", handlers.GetProductById)
	// products.Put("/:id", handlers.UpdateProduct)

	app.Use(func(c *fiber.Ctx) error {
		return c.Status(404).JSON(fiber.Map{
			"code":    404,
			"message": "404: Not Found",
		})
	})

}
