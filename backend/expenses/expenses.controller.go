package expenses

import (
	"context"
	"encoding/json"

	"github.com/Ayrat-Kh/expenso-app/backend/helpers"
	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/log"
	"github.com/google/uuid"
	"gorm.io/gorm"
)

// @Summary      Create an expense for a logged in user
// @Description  Create an expense for a logged in user
// @Tags         expenses
// @Accept       json
// @Produce      json
// @Param        Authorization header string true "Bearer"
// @Param        data body expenses.ExpenseUpsertDto true "Update expense model"
// @Success      200  {object} expenses.ExpenseResult
// @Failure      400  {object} helpers.ErrorResponse
// @Router       /v1/expenses [post]
func postExpenses(app *fiber.App) {
	app.Post("/v1/expenses", func(c *fiber.Ctx) error {
		ctx := helpers.BuildAppContext(c, context.Background())
		requestId := helpers.GetCtxRequestId(ctx)
		updateExpenseDto := ExpenseUpsertDto{}

		if err := json.Unmarshal(c.Body(), &updateExpenseDto); err != nil {
			log.Errorf("%s %s %s", requestId, helpers.CODE_INVALID_BODY, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_INVALID_BODY, "Invalid body"),
			)
		}

		userId, _ := helpers.GetCtxAuthUserId(ctx)
		userLoginResult, err := ExpenseService.CreateExpense(userId, updateExpenseDto, ctx)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_GOOGLE_LOGIN_FAILED, "Couldn't do a google login"),
			)
		}

		return c.JSON(userLoginResult)
	})
}

// @Summary      Update an expense for a logged in user
// @Description  Update an expense for a logged in user
// @Tags         expenses
// @Accept       json
// @Produce      json
// @Param        Authorization header string true "Bearer"
// @Param        expenseId   path      string  true  ":expenseId"
// @Param        data body expenses.ExpenseUpsertDto true "Update expense model"
// @Success      200  {object} expenses.ExpenseResult
// @Failure      400  {object} helpers.ErrorResponse
// @Router       /v1/expenses/{expenseId} [put]
func putExpenses(app *fiber.App) {
	// handler
	app.Put("/v1/expenses/:expenseId", func(c *fiber.Ctx) error {
		ctx := helpers.BuildAppContext(c, context.Background())
		requestId := helpers.GetCtxRequestId(ctx)

		expenseId, err := uuid.Parse(c.Params("expenseId"))
		updateExpenseDto := ExpenseUpsertDto{}

		if err := json.Unmarshal(c.Body(), &updateExpenseDto); err != nil {
			log.Errorf("%s %s %s", requestId, helpers.CODE_INVALID_BODY, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_INVALID_BODY, "Invalid body"),
			)
		}

		userId, _ := helpers.GetCtxAuthUserId(ctx)
		userLoginResult, err := ExpenseService.UpdateExpense(userId, expenseId, updateExpenseDto, ctx)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_GOOGLE_LOGIN_FAILED, "Couldn't do a google login"),
			)
		}

		return c.JSON(userLoginResult)
	})
}

// @Summary      Get expenses for the logged in user
// @Description  Get expenses for the logged in user
// @Tags         expenses
// @Accept       json
// @Produce      json
// @Param        Authorization header string true "Bearer"
// @Param        page   query      int  true  "page or 1 by default"
// @Param        pageSize   query      int  true  "pageSize or 300 by default"
// @Success      200  {object} expenses.ExpensesResult
// @Failure      400  {object} helpers.ErrorResponse
// @Router       /v1/expenses [get]
func getExpenses(app *fiber.App) {
	app.Get("/v1/expenses", func(c *fiber.Ctx) error {
		ctx := helpers.BuildAppContext(c, context.Background())
		requestId := helpers.GetCtxRequestId(ctx)
		page := c.QueryInt("page", 1)
		pageSize := c.QueryInt("pageSize", 300)

		userId, _ := helpers.GetCtxAuthUserId(ctx)
		expenses, err := ExpenseService.GetUserExpenses(userId, page, pageSize, ctx)

		if err != nil {
			log.Errorf("%s %s", requestId, err)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_INVALID_REQUEST, "Could not fetch user expenses"),
			)
		}

		return c.JSON(expenses)
	})
}

// @Summary      Get expense by expenseId
// @Description  Get expense by expenseId
// @Tags         expenses
// @Accept       json
// @Produce      json
// @Param        Authorization header string true "Bearer"
// @Param        expenseId   path      string  true  ":expenseId"
// @Success      200  {object} expenses.ExpenseResult
// @Failure      400  {object} helpers.ErrorResponse
// @Router       /v1/expenses/{expenseId} [get]
func getExpense(app *fiber.App) {
	app.Get("/v1/expenses/:expenseId", func(c *fiber.Ctx) error {
		ctx := helpers.BuildAppContext(c, context.Background())
		requestId := helpers.GetCtxRequestId(ctx)
		expenseId, err := uuid.Parse(c.Params("expenseId"))

		if err != nil {
			log.Errorf("%s %s expenseId is not provided", requestId, helpers.CODE_INVALID_PARAM)
			return c.Status(400).JSON(
				helpers.BuildErrorResponse(helpers.CODE_INVALID_PARAM, "expenseId is not provided"),
			)
		}

		userId, _ := helpers.GetCtxAuthUserId(ctx)
		expense, err := ExpenseService.GetUserExpense(userId, expenseId, ctx)

		if err != nil {
			log.Errorf("%s %s", requestId, err)

			status := 400

			if err == gorm.ErrRecordNotFound {
				status = 404
			}

			return c.Status(status).JSON(
				helpers.BuildErrorResponse(helpers.CODE_GOOGLE_LOGIN_FAILED, "Couldn't get expense"),
			)
		}

		return c.JSON(expense)
	})
}

func InitPrivate(app *fiber.App) {
	postExpenses(app)
	getExpense(app)
	getExpenses(app)
	putExpenses(app)
}
