package expenses

import (
	"context"

	guuid "github.com/google/uuid"
)

func (expenseDb *ExpenseItemDb) ToExpenseDto(expenseDto *ExpenseItemDto) {
	expenseDto.CurrencyCode = expenseDb.CurrencyCode
	expenseDto.Name = expenseDb.Name
	expenseDto.Price = expenseDb.Price
}

func (expenseItemDto *ExpenseItemDto) ToExpenseDto(expenseDb *ExpenseItemDb) {
	expenseDb.CurrencyCode = expenseItemDto.CurrencyCode
	expenseDb.Name = expenseItemDto.Name
	expenseDb.Price = expenseItemDto.Price
}

func (expenseDb *ExpenseDb) ToExpenseDto(expenseDto *ExpenseDto) {
	expenseDto.CreatedAt = expenseDb.CreatedAt
	expenseDto.UpdatedAt = expenseDb.UpdatedAt
	expenseDto.Id = expenseDb.Id
	expenseDto.UserId = expenseDb.UserId
	expenseDto.Total = expenseDb.Total
	expenseDto.CurrencyCode = expenseDb.CurrencyCode
	expenseDto.Category = expenseDb.Category
	expenseDto.Name = expenseDb.Name

	expenseDetails := make([]ExpenseItemDto, len(expenseDb.Details))
	for i := range expenseDetails {
		expenseDb.Details[i].ToExpenseDto(&expenseDetails[i])
	}
	expenseDto.Details = expenseDetails
}

func (expenseDto *ExpenseDto) ToExpenseDb(expenseDb *ExpenseDb) {

	expenseDb.CreatedAt = expenseDto.CreatedAt
	expenseDb.UpdatedAt = expenseDto.UpdatedAt
	expenseDb.Id = expenseDto.Id
	expenseDb.UserId = expenseDto.UserId
	expenseDb.Total = expenseDto.Total
	expenseDb.CurrencyCode = expenseDto.CurrencyCode
	expenseDb.Category = expenseDto.Category
	expenseDb.Name = expenseDto.Name

	expenseDetailsDb := make([]ExpenseItemDb, len(expenseDto.Details))
	for i := range expenseDetailsDb {
		expenseDto.Details[i].ToExpenseDto(&expenseDetailsDb[i])
	}
	expenseDb.Details = expenseDetailsDb
}

func (expenseUpdateDto ExpenseUpsertDto) ToToExpenseDb(expense *ExpenseDb) {
	expenseDetails := make([]ExpenseItemDb, len(expenseUpdateDto.Details))
	for i := range expenseDetails {
		expenseUpdateDto.Details[i].ToExpenseDto(&expenseDetails[i])
	}
	expense.Details = expenseDetails
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

	expenses := make([]ExpenseDto, len(resultDb.Expenses))
	for i := range expenses {
		resultDb.Expenses[i].ToExpenseDto(&expenses[i])
	}

	return ExpensesResult{
		Expenses:         expenses,
		PaginationResult: resultDb.PaginationResult,
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
