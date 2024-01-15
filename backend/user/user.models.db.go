package user

import (
	guuid "github.com/google/uuid"
)

type UserDb struct {
	Id        guuid.UUID `gorm:"primaryKey;type:uuid;default:gen_random_uuid()"`
	CreatedAt int64      `gorm:"autoCreateTime"`
	UpdatedAt int64      `gorm:"autoUpdateTime:milli"`
	FirstName string
	LastName  string
	Alias     string
	Email     string
	Password  string
	GoogleId  string ` gorm:"index:unique,where:'googleId' != null"`
}

type UpdateUserDb struct {
	FirstName string
	LastName  string
	Alias     string
}
