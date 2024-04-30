#include <boost/asio/spawn.hpp>

#include "auth.handlers.hpp"
#include "accounts/services/appDependencies.hpp"
#include "accounts/auth/auth.utils.hpp"
#include "accounts/utils/readRequestJson.hpp"
#include "accounts/utils/jsonSerialize.hpp"
#include "accounts/utils/error.hpp"

void accounts::handleGoogleLogin(uWS::HttpResponse<false> *rawResponse, uWS::HttpRequest *_req)
{
	auto handler = [](uWS::HttpResponse<false> *_res, uWS::HttpRequest *, GoogleLoginRequest parsedBody) mutable
	{
		if (abortIfValidationFailed(_res, validateGoogleLoginBody(parsedBody)))
		{
			return;
		}

		boost::asio::co_spawn(
			AppDependencies::instance().ioContext,
			[&parsedBody](uWS::HttpResponse<false> *res) mutable -> boost::asio::awaitable<void>
			{
				auto &&result = co_await AppDependencies::instance().authService->googleAuth(parsedBody.idToken);
				if (abortIfAppError(res, &result))
				{
					co_return;
				}

				UserLoginResult &&loginResult = std::get<UserLoginResult>(std::move(result));

				res
					->writeHeader("Content-Type", "application/json")
					->end(boost::json::serialize(std::move(boost::json::value_from(
						std::move(loginResult)))));
			}(_res),
			boost::asio::detached);
	};

	RequestJsonBodyReader reader;
	reader.read<GoogleLoginRequest>(rawResponse, _req, handler);
}
