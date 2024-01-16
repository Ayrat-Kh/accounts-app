package expenses

import (
	"context"

	guuid "github.com/google/uuid"
)

func (expenseDb *ExpenseDb) ToExpenseDto(expenseDto *ExpenseDto) {
	expenseDto.CreatedAt = expenseDb.CreatedAt
	expenseDto.UpdatedAt = expenseDb.UpdatedAt
	expenseDto.Id = expenseDb.Id
	expenseDto.UserId = expenseDb.UserId
	expenseDto.Details = expenseDb.Details
	expenseDto.Total = expenseDb.Total
	expenseDto.CurrencyCode = expenseDb.CurrencyCode
	expenseDto.Category = expenseDb.Category
	expenseDto.Name = expenseDb.Name
}

func (expenseDto *ExpenseDto) ToExpenseDb(expenseDb *ExpenseDb) {

	expenseDb.CreatedAt = expenseDto.CreatedAt
	expenseDb.UpdatedAt = expenseDto.UpdatedAt
	expenseDb.Id = expenseDto.Id
	expenseDb.UserId = expenseDto.UserId
	expenseDb.Details = expenseDto.Details
	expenseDb.Total = expenseDto.Total
	expenseDb.CurrencyCode = expenseDto.CurrencyCode
	expenseDb.Category = expenseDto.Category
	expenseDb.Name = expenseDto.Name

}

func (expenseUpdateDto ExpenseUpsertDto) ToToExpenseDb(expense *ExpenseDb) {
	expense.Details = expenseUpdateDto.Details
	expense.Total = expenseUpdateDto.Total
	expense.CurrencyCode = expenseUpdateDto.CurrencyCode
	expense.Category = expenseUpdateDto.Category
	expense.Name = expenseUpdateDto.Name
}

type ExpenseServiceInterface interface {
	GetUserExpenses(userId guuid.UUID, page int, pageSize int, context context.Context) (ExpensesResult, error)
	GetUserExpense(userId guuid.UUID, expenseId guuid.UUID, context context.Context) (ExpenseResult, error)
	CreateExpense(userId guuid.UUID, expense ExpenseUpsertDto, context context.Context) (ExpenseResult, error)
	UpdateExpense(userId guuid.UUID, expenseId guuid.UUID, expense ExpenseUpsertDto, context context.Context) (ExpenseResult, error)
}

type expenseService struct {
	expensesRepository
}

func (service *expenseService) GetUserExpenses(userId guuid.UUID, page int, pageSize int, context context.Context) (ExpensesResult, error) {
	resultDb, err := service.expensesRepository.GetUserExpenses(userId, page, pageSize)

	if err != nil {

	}

	expences := make([]ExpenseDto, len(resultDb))
	for i := range expences {
		resultDb[i].ToExpenseDto(&expences[i])
	}

	return ExpensesResult{
		Expenses: expences,
	}, err
}

func (service *expenseService) GetUserExpense(userId guuid.UUID, expenseId guuid.UUID, context context.Context) (ExpenseResult, error) {
	result := ExpenseResult{}

	resultDto, err := service.expensesRepository.GetExpenseByUserId(userId, expenseId)
	resultDto.ToExpenseDto(&result.Expense)

	return result, err
}

func (service *expenseService) CreateExpense(userId guuid.UUID, expense ExpenseUpsertDto, context context.Context) (ExpenseResult, error) {
	expenseDb := ExpenseDb{}
	expenseDb.UserId = userId

	expense.ToToExpenseDb(&expenseDb)

	createdExpenseDb, err := service.expensesRepository.UpsertExpense(userId, expenseDb)

	expenseDto := ExpenseDto{}

	createdExpenseDb.ToExpenseDto(&expenseDto)

	return ExpenseResult{
		Expense: expenseDto,
	}, err
}

func (service *expenseService) UpdateExpense(userId guuid.UUID, expenseId guuid.UUID, expense ExpenseUpsertDto, context context.Context) (ExpenseResult, error) {
	expenseDb := ExpenseDb{}
	expenseDb.Id = expenseId
	expense.ToToExpenseDb(&expenseDb)

	updatedExpenseDb, err := service.expensesRepository.UpsertExpense(userId, expenseDb)

	expenseDto := ExpenseDto{}

	updatedExpenseDb.ToExpenseDto(&expenseDto)

	return ExpenseResult{
		Expense: expenseDto,
	}, err
}

var ExpenseService ExpenseServiceInterface = &expenseService{
	expensesRepository: expensesRepository{},
}
