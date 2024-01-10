package helpers

import "github.com/gofiber/fiber/v2"

const (
	CODE_NOT_FOUND           = "NOT_FOUND"
	CODE_INVALID_BODY        = "INVALID_BODY"
	CODE_GOOGLE_LOGIN_FAILED = "GOOGLE_LOGIN_FAILED"
)

func BuildErrorResponse(code string, message string) fiber.Map {
	return fiber.Map{
		"code":    code,
		"message": message,
	}
}
