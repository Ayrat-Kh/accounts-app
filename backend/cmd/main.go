package main

import (
	"log"

	"github.com/Ayrat-Kh/expenso-app/backend/database"
	databasemigrate "github.com/Ayrat-Kh/expenso-app/backend/database/database-migrate"
	"github.com/Ayrat-Kh/expenso-app/backend/router"
	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/middleware/cors"
	"github.com/joho/godotenv"
)

// @title Expenso-app API
// @version 1.0
// @contact.name API Support
// @contact.email fiber@swagger.io
// @license.name Apache 2.0
// @license.url http://www.apache.org/licenses/LICENSE-2.0.html
// @BasePath /
func main() {
	godotenv.Load()
	app := fiber.New()
	app.Use(cors.New(cors.Config{
		AllowOrigins: "*", // comma string format e.g. "localhost, nikschaefer.tech"
		AllowHeaders: "Origin, Content-Type, Accept",
	}))

	router.Initalize(app)

	database.ConnectDb()
	databasemigrate.MigrateDb()

	log.Println("Staring the app")
	log.Fatal(app.Listen(":3000"))
}
