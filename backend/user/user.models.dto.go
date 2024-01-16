package user

import guuid "github.com/google/uuid"

type UserSettingsDto struct {
	DefaultCurrency string `json:"defaultCurrency"`
}

type UserDto struct {
	Id        guuid.UUID      `json:"id"`
	Email     string          `json:"email"`
	GoogleId  string          `json:"googleId"`
	FirstName string          `json:"firstName"`
	LastName  string          `json:"lastName"`
	Alias     string          `json:"alias"`
	Settings  UserSettingsDto `json:"settings"`
}

type UpdateUserDto struct {
	FirstName string         `json:"firstName"`
	LastName  string         `json:"lastName"`
	Alias     string         `json:"alias"`
	Settings  UserSettingsDb `json:"settings"`
}

type UserResult struct {
	User UserDto `json:"user"`
}
