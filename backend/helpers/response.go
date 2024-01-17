package helpers

const (
	CODE_NOT_FOUND           = "NOT_FOUND"
	CODE_INVALID_BODY        = "INVALID_BODY"
	CODE_INVALID_REQUEST     = "INVALID_REQUEST"
	CODE_INVALID_PARAM       = "INVALID_PARAM"
	CODE_GOOGLE_LOGIN_FAILED = "GOOGLE_LOGIN_FAILED"
	CODE_UNAUTHORIZED        = "CODE_UNAUTHORIZED"
)

type ErrorResponse struct {
	Code    string `json:"code" validate:"required"`
	Message string `json:"message" validate:"required"`
}

type PaginationResult struct {
	ItemsPerPage int `json:"itemsPerPage" validate:"required"`
	CurrentPage  int `json:"currentPage" validate:"required"`
	TotalPages   int `json:"totalPages" validate:"required"`
}

func BuildErrorResponse(code string, message string) ErrorResponse {
	return ErrorResponse{
		code,
		message,
	}
}
