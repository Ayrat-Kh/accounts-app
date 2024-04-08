#include "auth.handlers.hpp"
#include "services/app-dependencies.hpp"
#include "utils/read-request-json.hpp"
#include "auth/auth.utils.hpp"
#include "utils/jsonSerialize.hpp"
#include "utils/error.hpp"

void app::auth::handleGoogleLogin(uWS::HttpResponse<false> *_res, uWS::HttpRequest *req)
{
	auto handler = [](uWS::HttpResponse<false> *res, boost::json::value requestBody) mutable
	{
		auto &&reqBody = boost::json::try_value_to<app::auth::GoogleLoginRequest>(std::move(requestBody));

		if (reqBody.has_error())
		{
			app::error::abort(
				res,
				std::move(
					app::shared::AppError{
						.code = app::utils::enumToString(app::shared::AppErrorCode::PARSE_ERROR),
						.message = std::string(reqBody.error().message())}));
			return;
		}

		auto parsedBody = reqBody.value();

		if (app::error::abortIfValidationFailed(res, app::auth::validateGoogleLoginBody(parsedBody)))
		{
			return;
		}

		auto &&result = app::services::AppDependencies::instance().authService->googleAuth(parsedBody.idToken);
		if (app::error::abortIfAppError(res, &result))
		{
			return;
		}

		app::auth::UserLoginResult &&loginResult = std::get<app::auth::UserLoginResult>(std::move(result));

		res
			->writeHeader("Content-Type", "application/json")
			->end(boost::json::serialize(std::move(boost::json::value_from(
				loginResult))));
	};

	std::string _buffer;
	app::utils::requestBodyReader(_res, _buffer, handler);
}
