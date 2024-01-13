package helpers

import (
	"fmt"

	"github.com/gofiber/fiber/v2"
	"github.com/golang-jwt/jwt/v5"
)

func GetUserClaims(c *fiber.Ctx) (jwt.Claims, error) {
	userToken := c.Locals("user")

	if userToken == nil {
		return jwt.MapClaims{}, fmt.Errorf("Please provide jwt token")
	}

	return userToken.(*jwt.Token).Claims, nil
}
