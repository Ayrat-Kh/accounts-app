package auth

import guuid "github.com/google/uuid"

type UserDto struct {
	Id        guuid.UUID `json:"-"`
	Username  string     `json:"username"`
	Email     string     `json:"email"`
	CreatedAt int64      `gorm:"autoCreateTime" json:"-" `
	GoogleId  string     `json:"googleId"`
}

type GoogleLoginRequest struct {
	AccessToken string `json:"accessToken" xml:"accessToken" form:"accessToken"`
}

type UserLoginResult struct {
	User        UserDto `json:"user"`
	AccessToken string  `json:"accessToken"`
}

type UserResult struct {
	User UserDto `json:"user"`
}
