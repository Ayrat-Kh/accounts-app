package expenses

import (
	"database/sql"
	"math"

	"github.com/Ayrat-Kh/expenso-app/backend/database"
	"github.com/Ayrat-Kh/expenso-app/backend/helpers"
	guuid "github.com/google/uuid"
)

type expensesRepositoryInterface interface {
	GetUserExpenses(userId guuid.UUID, page uint32, pageSize uint32) (ExpensesDb, error)
	GetExpenseByUserId(expenseId guuid.UUID) (ExpenseDb, error)
	UpsertExpense(userId guuid.UUID, expense ExpenseDb) (ExpenseDb, error)
}

type expensesRepository struct {
}

func (rep *expensesRepository) GetUserExpenses(userId guuid.UUID, page int, pageSize int) (ExpensesDb, error) {
	userExpenses := []ExpenseDb{}

	query := database.DB.Where("user_id = @userId", sql.Named("userId", userId)).Order("created_at desc").Offset(page - 1).Limit(int(pageSize))

	result := query.Find(&userExpenses)

	var totalCount int64
	query.Count(&totalCount)

	return ExpensesDb{
		Expenses: userExpenses,
		PaginationResult: helpers.PaginationResult{
			CurrentPage:  page,
			TotalPages:   int(math.Ceil(float64(totalCount) / float64(pageSize))),
			ItemsPerPage: pageSize,
		},
	}, result.Error
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
