package auth

import (
	guuid "github.com/google/uuid"
)

type User struct {
	Id       guuid.UUID `gorm:"primaryKey" json:"-"`
	Username string     `json:"username"`
	Email    string     `json:"email"`
	Password string     `json:"-"`
	// Sessions  []Session  `gorm:"foreignKey:UserRefer; constraint:OnUpdate:CASCADE, OnDelete:CASCADE;" json:"-"`
	// Products  []Product  `gorm:"foreignKey:UserRefer; constraint:OnUpdate:CASCADE, OnDelete:CASCADE;" json:"-"`
	CreatedAt int64 `gorm:"autoCreateTime" json:"-" `
	UpdatedAt int64 `gorm:"autoUpdateTime:milli" json:"-"`
}

type Session struct {
	Id guuid.UUID `gorm:"primaryKey" json:"-"`
}
