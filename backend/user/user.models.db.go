package user

import (
	"database/sql/driver"
	"encoding/json"

	guuid "github.com/google/uuid"
)

type UserSettingsDb struct {
	DefaultCurrency string
}

func (j UserSettingsDb) Value() (driver.Value, error) {
	valueString, err := json.Marshal(j)
	if err != nil {
		valueString, err = json.Marshal(&UserSettingsDb{})
	}

	return string(valueString), err
}

func (j *UserSettingsDb) Scan(value interface{}) error {
	if err := json.Unmarshal(value.([]byte), &j); err != nil {
		value = &UserSettingsDb{}
		return nil
	}
	return nil
}

type UserDb struct {
	Id        guuid.UUID `gorm:"primaryKey;type:uuid;default:gen_random_uuid()"`
	CreatedAt int64      `gorm:"autoCreateTime"`
	UpdatedAt int64      `gorm:"autoUpdateTime:milli"`
	FirstName string     `gorm:"type:varchar(100);"`
	LastName  string     `gorm:"type:varchar(100);"`
	Alias     string     `gorm:"type:varchar(100);"`
	Email     string     `gorm:"type:varchar(100);"`
	Password  string
	GoogleId  string         ` gorm:"index:unique,where:'googleId' != null"`
	Settings  UserSettingsDb `gorm:"type:jsonb"`
}

type UpdateUserDb struct {
	FirstName string `gorm:"type:varchar(100);"`
	LastName  string `gorm:"type:varchar(100);"`
	Alias     string `gorm:"type:varchar(100);"`
}
