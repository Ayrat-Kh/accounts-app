package user

import guuid "github.com/google/uuid"

type UserDto struct {
	Id        guuid.UUID `json:"id"`
	Email     string     `json:"email"`
	GoogleId  string     `json:"googleId"`
	FirstName string     `json:"firstName"`
	LastName  string     `json:"lastName"`
	Alias     string     `json:"alias"`
}

type UpdateUserDto struct {
	FirstName string `json:"firstName"`
	LastName  string `json:"lastName"`
	Alias     string `json:"alias"`
}

type UserResult struct {
	User UserDto `json:"user"`
}
