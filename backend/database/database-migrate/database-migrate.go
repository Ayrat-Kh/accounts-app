package databasemigrate

import (
	"log"

	"github.com/Ayrat-Kh/expenso-app/backend/auth"
	"github.com/Ayrat-Kh/expenso-app/backend/database"
)

func MigrateDb() {
	err := database.DB.AutoMigrate(&auth.UserDb{})
	if err != nil {
		log.Fatal(err)
	}
}
