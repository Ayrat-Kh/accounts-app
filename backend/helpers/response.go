package helpers

import "github.com/gofiber/fiber/v2"

const (
	CODE_NOT_FOUND = "NOT_FOUND"
)

func BuildErrorResponse(code string, message string) fiber.Map {
	return fiber.Map{
		"code":    code,
		"message": message,
	}
}
