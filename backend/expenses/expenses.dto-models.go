package expenses

import (
	guuid "github.com/google/uuid"
	_ "github.com/lib/pq"
)

type ExpenseItemDto struct {
	Name         string  `json:"name"`
	Price        float64 `json:"price"`
	CurrencyCode string  `json:"currencyCode"`
}

type ExpenseItemsDto []ExpenseItemDto

type ExpenseDto struct {
	Id           guuid.UUID     `json:"id"`
	CreatedAt    int64          `json:"createdAt"`
	UpdatedAt    int64          `json:"updatedAt"`
	UserId       guuid.UUID     `json:"userId"`
	Details      ExpenseItemsDb `json:"details"`
	Total        float64        `json:"total"`
	CurrencyCode string         `json:"currencyCode"`
	Category     string         `json:"category"`
	Name         string         `json:"name"`
}
