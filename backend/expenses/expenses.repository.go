package expenses

import (
	"database/sql"

	"github.com/Ayrat-Kh/expenso-app/backend/database"
	guuid "github.com/google/uuid"
)

type expensesRepositoryInterface interface {
	GetUserExpenses(userId guuid.UUID, page uint32, pageSize uint32) ([]ExpenseDb, error)
	GetExpenseByUserId(expenseId guuid.UUID) (ExpenseDb, error)
	UpsertExpense(userId guuid.UUID, expense ExpenseDb) (ExpenseDb, error)
}

type expensesRepository struct {
}

func (rep *expensesRepository) GetUserExpenses(userId guuid.UUID, page int, pageSize int) ([]ExpenseDb, error) {
	userExpenses := []ExpenseDb{}

	result := database.DB.Where("user_id = @userId", sql.Named("userId", userId)).Order("created_at desc").Offset(page - 1).Limit(int(pageSize)).Find(&userExpenses)

	return userExpenses, result.Error
}

func (rep *expensesRepository) GetExpenseByUserId(userId guuid.UUID, expenseId guuid.UUID) (ExpenseDb, error) {
	userExpense := ExpenseDb{}

	result := database.DB.Where("id = @expenseId AND user_id = @userId", sql.Named("expenseId", expenseId), sql.Named("userId", userId)).First(&userExpense)

	return userExpense, result.Error
}

func (rep *expensesRepository) UpsertExpense(userId guuid.UUID, expense ExpenseDb) (ExpenseDb, error) {
	updateExpense := ExpenseDb{}

	result := database.DB.Where("user_id = @userId", sql.Named("userId", userId)).Save(expense)

	return updateExpense, result.Error
}
