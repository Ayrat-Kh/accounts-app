package helpers

import (
	"fmt"

	"github.com/gofiber/fiber/v2"
)

func GetRequestId(c *fiber.Ctx) string {
	return fmt.Sprintf("%v", c.Locals("requestid"))
}
