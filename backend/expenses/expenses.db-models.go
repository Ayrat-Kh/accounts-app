package expenses

import (
	"database/sql/driver"
	"encoding/json"

	"github.com/Ayrat-Kh/expenso-app/backend/user"
	guuid "github.com/google/uuid"
	_ "github.com/lib/pq"
)

type ExpenseItemDb struct {
	Name         string
	Price        float64
	CurrencyCode string
}

type ExpenseItemsDb []ExpenseItemDb

func (j ExpenseItemsDb) Value() (driver.Value, error) {
	valueString, err := json.Marshal(j)
	if err != nil {
		valueString, err = json.Marshal(&ExpenseItemsDb{})
	}

	return string(valueString), err
}

func (j ExpenseItemsDb) Scan(value interface{}) error {
	if err := json.Unmarshal(value.([]byte), &j); err != nil {
		value = &ExpenseItemsDb{}
		return nil
	}
	return nil
}

type ExpenseDb struct {
	Id           guuid.UUID     `gorm:"primaryKey;type:uuid;default:gen_random_uuid()"`
	CreatedAt    int64          `gorm:"autoCreateTime"`
	UpdatedAt    int64          `gorm:"autoUpdateTime:milli"`
	UserId       guuid.UUID     `gorm:"references:Id"`
	UserDb       user.UserDb    `gorm:"references:Id;foreignKey:UserId;constraint:OnUpdate:CASCADE;OnDelete:CASCADE;"`
	Details      ExpenseItemsDb `gorm:"type:jsonb"`
	Total        float64
	CurrencyCode string `gorm:"type:varchar(100);"`
	Category     string `gorm:"type:varchar(100);"`
	Name         string `gorm:"type:varchar(100);"`
}
