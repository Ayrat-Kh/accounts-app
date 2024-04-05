#include "auth.handlers.hpp"

#include "services/app-dependencies.hpp"
#include "utils/read-request-json.hpp"
#include "auth/auth.utils.hpp"

#include "utils/date.hpp"

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

		// std::chrono::system_clock::time_point createdAt;
		// std::chrono::system_clock::time_point updatedAt;
		boost::json::object user(
			{
				{"alias", std::move(loginResult.user.alias)},
				{"email", std::move(loginResult.user.email)},
				{"id", std::move(loginResult.user.id)},
				{"firstName", std::move(loginResult.user.firstName)},
				{"lastName", std::move(loginResult.user.lastName)},
				{"googleId", std::move(loginResult.user.googleId)},
				{"createdAt", app::utils::timePointToString(loginResult.user.createdAt)},
				{"settings", std::move(boost::json::object({{"defaultCurrency", loginResult.user.settings.defaultCurrency}}))},
			});

		boost::json::object gl(
			{
				{"accessToken", std::move(loginResult.accessToken)},
				{"sessionToken", std::move(loginResult.sessionToken)},
				{"user", std::move(user)},
			});

		res
			->writeHeader("Content-Type", "application/json")
			->end(boost::json::serialize(gl));
	};

	std::string _buffer;

	app::utils::requestBodyReader(_res, _buffer, handler);
	// _buffer.reserve(4096);

	// res->onAborted(
	// 	   []() {})
	// 	->onData(
	// 		[res, &_buffer, handler](std::string_view data, bool isLast) mutable
	// 		{
	// 			_buffer.append(data);

	// 			if (!isLast)
	// 			{
	// 				return;
	// 			}

	// 			try
	// 			{
	// 				auto value = boost::json::parse(_buffer);

	// 				handler(std::move(value));
	// 			}
	// 			catch (std::exception &ex)
	// 			{
	// 				// ToDo log me later
	// 				std::cerr << "Request caught error " << ex.what() << std::endl;

	// 				boost::json::object obj({{"message", "Invalid input json object"}});

	// 				res
	// 					->writeHeader("Content-Type", "application/json")
	// 					->writeStatus("400")
	// 					->end(boost::json::serialize(obj));
	// 			}
	// 		});
}
