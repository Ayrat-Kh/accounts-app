#include <variant>

#include <boost/json.hpp>

#include "auth/auth.service.hpp"
#include "auth/auth.utils.hpp"
#include "users/users.repository.hpp"
#include "services/mongo-access.hpp"
#include "services/google-login.service.hpp"
#include "services/jwt.service.hpp"
#include "utils/read-request-json.hpp"
#include "utils/error.hpp"

#include <App.h>

int main(int argc, char *argv[])
{
    // ToDo config section - hardcoded for now. later read from env
    int32_t appPort = 3000;
    std::string secretKey = "secret super duper key";
    app::services::ConfigureMongoInstance(mongocxx::uri("mongodb://localhost:27017"));

    // dependency section
    std::shared_ptr<app::services::IGoogleLoginService> googleLoginService = std::make_shared<app::services::GoogleLoginServiceImpl>();
    std::shared_ptr<app::services::IJwtService> jwtService = std::make_shared<app::services::JwtServiceImpl>(secretKey);
    std::shared_ptr<app::users::IUsersRepository> userRepo = std::make_shared<app::users::UsersRepositoryImpl>();
    std::shared_ptr<app::auth::IAuthService> authService = std::make_shared<app::auth::AuthServiceImpl>(googleLoginService, userRepo, jwtService);

    // server section

    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
    uWS::App()
        .get(
            "/health-check",
            [](uWS::HttpResponse<false> *res, auto *req)
            {
                res->end("Server is working fine");
            })
        .post(
            "/login/google-auth",
            [authService](uWS::HttpResponse<false> *res, auto *req) mutable
            {
                auto handler = [authService, res](boost::json::value requestBody) mutable
                {
                    auto reqBody = app::auth::getGoogleLoginBodyFromJson(std::move(requestBody));

                    auto validationResult = app::auth::validateGoogleLoginBody(reqBody);

                    if (app::error::abortIfValidationFailed(res, std::move(validationResult)))
                    {
                        return;
                    }

                    auto result = authService.get()->googleAuth(reqBody.idToken);

                    if (app::error::abortIfAppError(res, &result))
                    {
                        return;
                    }

                    app::auth::UserLoginResult loginResult = std::get<app::auth::UserLoginResult>(result);

                    boost::json::object user(
                        {{
                             "alias",
                             std::move(loginResult.user.alias),
                         },
                         {"email", std::move(loginResult.user.email)}});

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
                _buffer.reserve(4096);

                res->onAborted(
                       []() {})
                    ->onData(
                        [res, _buffer, handler](std::string_view data, bool isLast) mutable
                        {
                            _buffer.append(data);

                            if (!isLast)
                            {
                                return;
                            }

                            try
                            {

                                auto value = boost::json::parse(_buffer);

                                handler(std::move(value));
                            }
                            catch (std::exception &ex)
                            {
                                // ToDo log me later
                                std::cerr << "Request caught error " << ex.what() << std::endl;

                                boost::json::object obj({{"message", "Invalid input json object"}});

                                res
                                    ->writeHeader("Content-Type", "application/json")
                                    ->writeStatus("400")
                                    ->end(boost::json::serialize(obj));
                            }
                        });
            })
        .listen(
            appPort,
            [=](auto *listen_socket)
            {
                if (listen_socket)
                {
                    std::cout << "Listening on port " << appPort << std::endl;
                }
            })
        .run();

    return 0;
}