#include "auth.handlers.hpp"
#include "services/app-dependencies.hpp"
#include "utils/read-request-json.hpp"
#include "auth/auth.utils.hpp"
#include "utils/jsonSerialize.hpp"
#include "utils/error.hpp"

void app::auth::handleGoogleLogin(uWS::HttpResponse<false> *_res, uWS::HttpRequest *req)
{
	auto handler = [](uWS::HttpResponse<false> *res, app::auth::GoogleLoginRequest parsedBody) mutable
	{
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

	app::utils::RequestJsonBodyReader reader;
	reader.read<app::auth::GoogleLoginRequest>(_res, handler);
}
