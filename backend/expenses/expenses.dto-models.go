package expenses

import (
	guuid "github.com/google/uuid"
	_ "github.com/lib/pq"
)

type ExpenseItemDto struct {
	Name         string  `json:"name" validate:"required"`
	Price        float64 `json:"price" validate:"required"`
	CurrencyCode string  `json:"currencyCode" validate:"required"`
}

type ExpenseItemsDto []ExpenseItemDto

type ExpenseUpsertDto struct {
	Details      ExpenseItemsDto `json:"details" validate:"required"`
	Total        float64         `json:"total" validate:"required"`
	CurrencyCode string          `json:"currencyCode" validate:"required"`
	Category     string          `json:"category" validate:"required"`
	Name         string          `json:"name" validate:"required"`
}

type ExpenseDto struct {
	Id           guuid.UUID      `json:"id" validate:"required"`
	CreatedAt    int64           `json:"createdAt" validate:"required"`
	UpdatedAt    int64           `json:"updatedAt" validate:"required"`
	UserId       guuid.UUID      `json:"userId" validate:"required"`
	Details      ExpenseItemsDto `json:"details" validate:"required"`
	Total        float64         `json:"total" validate:"required"`
	CurrencyCode string          `json:"currencyCode" validate:"required"`
	Category     string          `json:"category" validate:"required"`
	Name         string          `json:"name" validate:"required"`
}

type ExpensesResult struct {
	Expenses []ExpenseDto `json:"expenses" validate:"required"`
}

type ExpenseResult struct {
	Expense ExpenseDto `json:"expense" validate:"required"`
}
