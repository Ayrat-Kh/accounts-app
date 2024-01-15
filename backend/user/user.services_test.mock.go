package user

import (
	guuid "github.com/google/uuid"
)

type mockUserRepository struct{}

func (t *mockUserRepository) GetUserById(userId guuid.UUID) (UserDb, error) {
	return UserDb{
		GoogleId:  "googleId",
		FirstName: "username",
	}, nil
}

func (t *mockUserRepository) UpdateUser(userId guuid.UUID, u UpdateUserDb) (UserDb, error) {
	return UserDb{
		FirstName: u.FirstName + "updated",
		LastName:  u.LastName + "updated",
		Alias:     u.Alias + "updated",
	}, nil

}

func (rep *mockUserRepository) UpsertUserByGoogleId(googleId string, user UserDb) (UserDb, error) {
	user.FirstName = user.FirstName + "updated"
	return user, nil
}

var MockUserService UserServiceInterface = &userService{
	userRepositoryInterface: &mockUserRepository{},
}
