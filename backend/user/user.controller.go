package user

import (
	"context"
	"encoding/json"

	"github.com/Ayrat-Kh/expenso-app/backend/helpers"
	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/log"
	guuid "github.com/google/uuid"
)

// Update user info
// @Summary      Update user info
// @Description  Update user info
// @Tags         user
// @Accept       json
// @Produce      json
// @Param        data body user.UpdateUserDto true "User update data"
// @Success      200  {object} user.UserResult
// @Failure      400  {object} helpers.ErrorResponse
// @Router       /v1/users/:userId [put]
func handlePutUser(app *fiber.App) {
	// handler
	app.Put("/v1/users/:userId", func(c *fiber.Ctx) error {
		ctx, err := helpers.BuildAuthAppContext(c, context.Background())
		requestId := helpers.GetCtxRequestId(ctx)
		updateUserDto := UpdateUserDto{}
		userId, err := guuid.Parse(c.Params("userId"))

		if err != nil {
			log.Errorf("%s %s %s", requestId, helpers.CODE_INVALID_PARAM, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_INVALID_PARAM, "Invalid userId"),
			)
		}

		if err := json.Unmarshal(c.Body(), &updateUserDto); err != nil {
			log.Errorf("%s %s %s", requestId, helpers.CODE_INVALID_BODY, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_INVALID_BODY, "Invalid body"),
			)
		}

		userResult, err := UserService.UpdateUser(userId, updateUserDto, ctx)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_GOOGLE_LOGIN_FAILED, "Couldn't do a google login"),
			)
		}

		return c.JSON(userResult)
	})
}

// Get user info
// @Summary      Get user info
// @Description  Get user info
// @Tags         user
// @Param        Authorization header string true "Bearer"
// @Produce      json
// @Success      200  {object} user.UserResult
// @Failure      400  {object} helpers.ErrorResponse
// @Router       /v1/users/:userId [get]
func handleGetUser(app *fiber.App) {

	// handler
	app.Get("/v1/users/:userId", func(c *fiber.Ctx) error {
		ctx, err := helpers.BuildAuthAppContext(c, context.Background())
		requestId := helpers.GetCtxRequestId(ctx)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_UNAUTHORIZED, err.Error()),
			)
		}

		paramUserId := c.Params("userId")
		userId, err := helpers.GetCtxAuthUserId(ctx)

		if paramUserId != "me" {
			userId, err = guuid.Parse(paramUserId)

			if err != nil {
				log.Errorf("%s %s", requestId, err)
				return c.Status(400).JSON(
					helpers.BuildErrorResponse(helpers.CODE_INVALID_PARAM, "Invalid user param"),
				)
			}
		}

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_INVALID_BODY, "Invalid body"),
			)
		}

		user, err := UserService.GetUser(userId, ctx)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_INVALID_REQUEST, "Invalid body"),
			)
		}

		return c.JSON(user)
	})
}

func InitPrivate(app *fiber.App) {
	handleGetUser(app)
	handlePutUser(app)
}
