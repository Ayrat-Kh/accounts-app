package auth

import (
	guuid "github.com/google/uuid"
)

type UserDb struct {
	Id        guuid.UUID `json:"id" gorm:"primaryKey;type:uuid;default:gen_random_uuid()"`
	Username  string     `json:"username"`
	Email     string     `json:"email"`
	Password  string     `json:"-"`
	CreatedAt int64      `gorm:"autoCreateTime" json:"-"`
	UpdatedAt int64      `gorm:"autoUpdateTime:milli" json:"-"`
	GoogleId  string     `json:"googleId" gorm:"index:unique,where:'googleId' != null"`
}

type SessionDb struct {
	Id guuid.UUID `gorm:"primaryKey" json:"-"`
}
