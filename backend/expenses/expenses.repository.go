package expenses

import (
	"database/sql"

	"github.com/Ayrat-Kh/expenso-app/backend/database"
	guuid "github.com/google/uuid"
)

type expensesRepositoryInterface interface {
	GetUserExpenses(userId guuid.UUID, page uint32, pageSize uint32) ([]ExpenseDb, error)
	GetExpense(expenseId guuid.UUID) (ExpenseDb, error)
	UpsertExpense(expense ExpenseDb) (ExpenseDb, error)
}

type expensesRepository struct {
}

func (rep *expensesRepository) GetUserExpenses(userId guuid.UUID, page int, pageSize int) ([]ExpenseDb, error) {
	userExpenses := []ExpenseDb{}

	result := database.DB.Where("user_id = @userId", sql.Named("userId", userId)).Order("created_at desc").Offset(page - 1).Limit(int(pageSize)).Find(&userExpenses)

	return userExpenses, result.Error
}

func (rep *expensesRepository) GetExpense(expenseId guuid.UUID) (ExpenseDb, error) {
	userExpense := ExpenseDb{}

	result := database.DB.Where("id = @expenseId", sql.Named("expenseId", expenseId)).First(&userExpense)

	return userExpense, result.Error
}

func (rep *expensesRepository) UpsertExpense(expense ExpenseDb) (ExpenseDb, error) {
	updateExpense := ExpenseDb{}

	result := database.DB.Save(expense)

	return updateExpense, result.Error
}
