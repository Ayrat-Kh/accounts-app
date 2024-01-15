package auth

import (
	user "github.com/Ayrat-Kh/expenso-app/backend/user"
	guuid "github.com/google/uuid"
)

type SessionDb struct {
	Id        guuid.UUID `gorm:"primaryKey;type:uuid;default:gen_random_uuid()"`
	CreatedAt int64      `gorm:"autoCreateTime"`
	UpdatedAt int64      `gorm:"autoUpdateTime:milli"`

	UserId       guuid.UUID
	User         user.UserDb `gorm:"foreignKey:UserId;references:Id;constraint:OnUpdate:CASCADE;OnDelete:CASCADE"`
	AccessToken  string
	IsValid      bool
	SessionToken string
}
