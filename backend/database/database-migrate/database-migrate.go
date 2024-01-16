package databasemigrate

import (
	"log"

	"github.com/Ayrat-Kh/expenso-app/backend/auth"
	"github.com/Ayrat-Kh/expenso-app/backend/database"
	"github.com/Ayrat-Kh/expenso-app/backend/expenses"
	"github.com/Ayrat-Kh/expenso-app/backend/user"
)

func MigrateDb() {
	err := database.DB.AutoMigrate(&user.UserDb{}, &auth.SessionDb{}, &expenses.ExpenseDb{})
	if err != nil {
		log.Fatal(err)
	}
}
