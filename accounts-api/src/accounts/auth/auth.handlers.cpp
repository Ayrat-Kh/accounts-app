#include "auth.handlers.hpp"
#include "accounts/services/appDependencies.hpp"
#include "accounts/auth/auth.utils.hpp"
#include "accounts/utils/readRequestJson.hpp"
#include "accounts/utils/jsonSerialize.hpp"
#include "accounts/utils/error.hpp"

using namespace ::accounts::utils;
using namespace ::accounts::error;
using namespace ::accounts::services;

void accounts::auth::handleGoogleLogin(uWS::HttpResponse<false> *_res, uWS::HttpRequest *_req)
{
	auto handler = [](uWS::HttpResponse<false> *res, uWS::HttpRequest *, GoogleLoginRequest parsedBody) mutable
	{
		if (abortIfValidationFailed(res, validateGoogleLoginBody(parsedBody)))
		{
			return;
		}

		auto &&result = AppDependencies::instance().authService->googleAuth(parsedBody.idToken);
		if (abortIfAppError(res, &result))
		{
			return;
		}

		UserLoginResult &&loginResult = std::get<UserLoginResult>(std::move(result));

		res
			->writeHeader("Content-Type", "application/json")
			->end(boost::json::serialize(std::move(boost::json::value_from(
				std::move(loginResult)))));
	};

	RequestJsonBodyReader reader;
	reader.read<GoogleLoginRequest>(_res, _req, handler);
}
