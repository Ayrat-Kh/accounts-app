#include "auth.handlers.hpp"

#include "services/app-dependencies.hpp"
#include "utils/read-request-json.hpp"
#include "auth/auth.utils.hpp"
#include "users/users.utils.hpp"

void app::auth::handleGoogleLogin(uWS::HttpResponse<false> *_res, uWS::HttpRequest *req)
{
	auto handler = [](uWS::HttpResponse<false> *res, boost::json::value requestBody) mutable
	{
		auto reqBody = app::auth::getGoogleLoginBodyFromJson(std::move(requestBody));

		auto validationResult = app::auth::validateGoogleLoginBody(reqBody);
		if (app::error::abortIfValidationFailed(res, std::move(validationResult)))
		{
			return;
		}

		auto result = app::services::AppDependencies::instance().authService->googleAuth(reqBody.idToken);
		if (app::error::abortIfAppError(res, &result))
		{
			return;
		}

		app::auth::UserLoginResult loginResult = std::get<app::auth::UserLoginResult>(result);

		boost::json::object gl(
			{
				{"accessToken", std::move(loginResult.accessToken)},
				{"sessionToken", std::move(loginResult.sessionToken)},
				{"user", std::move(app::users::toJsonUserDb(std::move(loginResult.user)))},
			});

		res
			->writeHeader("Content-Type", "application/json")
			->end(boost::json::serialize(gl));
	};

	std::string _buffer;

	app::utils::requestBodyReader(_res, _buffer, handler);
}
