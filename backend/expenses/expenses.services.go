package expenses

import (
	"context"

	guuid "github.com/google/uuid"
)

func MapExpenseDbToExpenseDto(expense ExpenseDb) ExpenseDto {
	return ExpenseDto{
		CreatedAt:    expense.CreatedAt,
		UpdatedAt:    expense.UpdatedAt,
		Id:           expense.Id,
		UserId:       expense.UserId,
		Details:      expense.Details,
		Total:        expense.Total,
		CurrencyCode: expense.CurrencyCode,
		Category:     expense.Category,
		Name:         expense.Name,
	}
}

func MapExpenseDtoToExpenseDb(expense ExpenseDto) ExpenseDb {
	return ExpenseDb{
		CreatedAt:    expense.CreatedAt,
		UpdatedAt:    expense.UpdatedAt,
		Id:           expense.Id,
		UserId:       expense.UserId,
		Details:      expense.Details,
		Total:        expense.Total,
		CurrencyCode: expense.CurrencyCode,
		Category:     expense.Category,
		Name:         expense.Name,
	}
}

type ExpenseServiceInterface interface {
	GetUserExpenses(userId guuid.UUID, page int, pageSize int) ([]ExpenseDb, error)
	GetExpense(expenseId guuid.UUID) (ExpenseDb, error)
	UpsertExpense(expense ExpenseDb) (ExpenseDb, error)
}

type expenseService struct {
	expensesRepository
}

func (service *expenseService) GetUserExpenses(userId guuid.UUID, page int, pageSize int, context context.Context) ([]ExpenseDto, error) {
	resultDb, err := service.expensesRepository.GetUserExpenses(userId, page, pageSize)

	if err != nil {

	}

	result := make([]ExpenseDto, len(resultDb))
	for i := range result {
		result[i] = MapExpenseDbToExpenseDto(resultDb[i])
	}

	return result, err
}

func (service *expenseService) GetExpense(expenseId guuid.UUID, context context.Context) (ExpenseDto, error) {
	resultDto, err := service.expensesRepository.GetExpense(expenseId)

	return MapExpenseDbToExpenseDto(resultDto), err
}

func (service *expenseService) CreateExpense(expense ExpenseDto, context context.Context) (ExpenseDto, error) {
	resultDto, err := service.expensesRepository.UpsertExpense(
		MapExpenseDtoToExpenseDb(expense),
	)

	return MapExpenseDbToExpenseDto(resultDto), err
}

func (service *expenseService) UpdateExpense(expenseId guuid.UUID, expense ExpenseDto, context context.Context) (ExpenseDto, error) {
	expense.Id = expenseId

	resultDto, err := service.expensesRepository.UpsertExpense(
		MapExpenseDtoToExpenseDb(expense),
	)

	return MapExpenseDbToExpenseDto(resultDto), err
}
