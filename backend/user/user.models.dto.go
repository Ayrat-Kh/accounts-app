package user

import guuid "github.com/google/uuid"

type UserSettingsDto struct {
	DefaultCurrency string `json:"defaultCurrency"`
}

type UserDto struct {
	Id        guuid.UUID      `json:"id" validate:"required"`
	Email     string          `json:"email" validate:"required"`
	GoogleId  string          `json:"googleId"`
	FirstName string          `json:"firstName"`
	LastName  string          `json:"lastName"`
	Alias     string          `json:"alias"`
	Settings  UserSettingsDto `json:"settings" validate:"required"`
}

type UpdateUserDto struct {
	FirstName string         `json:"firstName"`
	LastName  string         `json:"lastName"`
	Alias     string         `json:"alias"`
	Settings  UserSettingsDb `json:"settings" validate:"required"`
}

type UserResult struct {
	User UserDto `json:"user" validate:"required"`
}
