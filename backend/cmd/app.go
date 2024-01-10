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
