package user

import (
	"context"
	"fmt"

	"github.com/Ayrat-Kh/expenso-app/backend/helpers"

	guuid "github.com/google/uuid"
)

type UserService struct {
}

type userServiceDI struct {
	userRepository
	UserService
}

func MapUpdateUserDbToUserDto(updateUserDto UpdateUserDto) UpdateUserDb {
	return UpdateUserDb{
		LastName:  updateUserDto.LastName,
		FirstName: updateUserDto.FirstName,
		Alias:     updateUserDto.Alias,
	}
}

func MapUserDbToUserDto(userDb UserDb) UserDto {
	return UserDto{
		Id:        userDb.Id,
		GoogleId:  userDb.GoogleId,
		LastName:  userDb.LastName,
		FirstName: userDb.FirstName,
		Alias:     userDb.Alias,
		Email:     userDb.Email,
	}
}

type UpdateUserInterface interface {
	UpdateUserDbInterface
}

func UpdateUser(userId guuid.UUID, user UpdateUserDto, userService UpdateUserInterface, ctx context.Context) (UserResult, error) {
	result := UserResult{}

	authUserId, err := helpers.GetCtxAuthUserId(ctx)

	if err != nil {
		return result, err
	}

	if authUserId != userId {
		return result, fmt.Errorf("User can update only own information")
	}

	userDb, err := userService.UpdateUser(userId, MapUpdateUserDbToUserDto(user))

	if err != nil {
		return result, err
	}

	result.User = MapUserDbToUserDto(userDb)

	return result, err
}

type HandleGetUserInterface interface {
	GetUserByIdDbInterface
}

func GetUser(userId guuid.UUID, userService HandleGetUserInterface, ctx context.Context) (UserResult, error) {
	result := UserResult{}
	user, err := userService.GetUserById(userId)

	if err != nil {
		return result, err
	}

	result.User = MapUserDbToUserDto(user)

	return result, nil
}
