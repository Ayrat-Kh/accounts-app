package helpers

import (
	"context"
	"fmt"
	"math/rand"

	"github.com/Ayrat-Kh/expenso-app/backend/constants"
	"github.com/gofiber/fiber/v2"
	"github.com/golang-jwt/jwt/v5"
	guuid "github.com/google/uuid"
)

func GetUserIdFromToken(jwtClaims jwt.Claims) (guuid.UUID, error) {
	claims := jwtClaims.(jwt.MapClaims)
	userId := claims[constants.APP_JWT_USER_ID].(string)
	return guuid.Parse(userId)

}

func GetUserClaims(c *fiber.Ctx) (jwt.Claims, error) {
	userToken := c.Locals("user")

	if userToken == nil {
		return jwt.MapClaims{}, fmt.Errorf("Please provide jwt token")
	}

	return userToken.(*jwt.Token).Claims, nil
}

func BuildAppContext(c *fiber.Ctx, ctx context.Context) context.Context {
	requestId := fmt.Sprintf("%v", c.Locals("requestid"))

	return context.WithValue(ctx, constants.APP_CTX_REQUEST_ID, requestId)
}

func BuildAuthContextWithValue(ctx context.Context, authUserId guuid.UUID) context.Context {
	return context.WithValue(ctx, constants.APP_CTX_AUTH_USER_ID, authUserId)
}

func BuildAuthAppContext(c *fiber.Ctx, ctx context.Context) (context.Context, error) {
	claims, err := GetUserClaims(c)

	if err != nil {
		return ctx, err
	}

	authUserId, err := GetUserIdFromToken(claims)

	if err != nil {
		return ctx, err
	}

	result := BuildAuthContextWithValue(ctx, authUserId)
	result = BuildAppContext(c, result)

	return result, nil
}

func GetCtxRequestId(ctx context.Context) string {
	result, ok := ctx.Value(constants.APP_CTX_REQUEST_ID).(string)

	if ok {
		return result
	}

	return "[No request id provided]"
}

func GetCtxAuthUserId(ctx context.Context) (guuid.UUID, error) {
	result, ok := ctx.Value(constants.APP_CTX_AUTH_USER_ID).(guuid.UUID)

	if ok {
		return result, nil
	}

	return result, fmt.Errorf("Please use BuildAuthAppContext to create request context or put endpoint to private context")
}

var letterRunes = []rune("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")

func RandStringRunes(n int) string {
	b := make([]rune, n)
	for i := range b {
		b[i] = letterRunes[rand.Intn(len(letterRunes))]
	}
	return string(b)
}
