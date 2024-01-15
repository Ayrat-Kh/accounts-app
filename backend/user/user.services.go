package user

import (
	"context"
	"fmt"

	"github.com/Ayrat-Kh/expenso-app/backend/helpers"

	guuid "github.com/google/uuid"
)

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

type UserServiceInterface interface {
	GetUser(userId guuid.UUID, ctx context.Context) (UserResult, error)
	UpdateUser(userId guuid.UUID, user UpdateUserDto, ctx context.Context) (UserResult, error)
	UpsertUserByGoogleId(googleId string, user UserDb) (UserDb, error)
}

type userService struct {
	userRepositoryInterface
}

func (dep *userService) UpdateUser(userId guuid.UUID, user UpdateUserDto, ctx context.Context) (UserResult, error) {
	result := UserResult{}

	authUserId, err := helpers.GetCtxAuthUserId(ctx)

	if err != nil {
		return result, err
	}

	if authUserId != userId {
		return result, fmt.Errorf("User can update only own information")
	}

	userDb, err := dep.userRepositoryInterface.UpdateUser(userId, MapUpdateUserDbToUserDto(user))

	if err != nil {
		return result, err
	}

	result.User = MapUserDbToUserDto(userDb)

	return result, err
}

func (dep *userService) GetUser(userId guuid.UUID, ctx context.Context) (UserResult, error) {
	result := UserResult{}

	user, err := dep.userRepositoryInterface.GetUserById(userId)

	if err != nil {
		return result, err
	}

	result.User = MapUserDbToUserDto(user)

	return result, nil
}

func (dep *userService) UpsertUserByGoogleId(googleId string, user UserDb) (UserDb, error) {
	return dep.userRepositoryInterface.UpsertUserByGoogleId(googleId, user)
}

var UserService UserServiceInterface = &userService{
	userRepositoryInterface: &userRepository{},
}
