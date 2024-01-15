package auth

import (
	"github.com/Ayrat-Kh/expenso-app/backend/shared/google"
	user "github.com/Ayrat-Kh/expenso-app/backend/user"
	guuid "github.com/google/uuid"
)

type mockUserSessionRepository struct{}

func (t *mockUserSessionRepository) CreateSession(session SessionDb) (SessionDb, error) {
	return session, nil
}

func (t *mockUserSessionRepository) InvalidateSessionForUser(userId guuid.UUID) error {
	return nil
}

type googleLoginMock struct{}

func (t *googleLoginMock) GetGoogleUser(accessToken string) (google.GoogleTokenInfo, error) {
	return google.GoogleTokenInfo{
		Sub: "SUB1",
	}, nil
}

var MockAuthService AuthServiceInterface = &authService{
	UserServiceInterface:           user.MockUserService,
	userSessionRepositoryInterface: &mockUserSessionRepository{},
	GoogleLoginInterface:           &googleLoginMock{},
}
