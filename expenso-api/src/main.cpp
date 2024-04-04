#include "auth/auth.service.hpp"
#include "users/users.repository.hpp"
#include "services/mongo-access.hpp"
#include "services/google-login.service.hpp"
#include "services/jwt.service.hpp"
#include "utils/read-request-json.hpp"

#include <App.h>

int main(int argc, char *argv[])
{
    // ToDo config section - hardcoded for now. later read from env
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
            "/users/:userId",
            [](uWS::HttpResponse<false> *res, auto *req)
            {
                auto userId = req->getParameter("userId");

                auto handler = [&](boost::json::value requestBody)
                {
                    // std::cout << "data" << ;
                };

                app::utils::ReadRequestJson(res, handler);

                res->end();
            })
        .listen(
            9001,
            [](auto *listen_socket)
            {
                if (listen_socket)
                {
                    std::cout << "Listening on port " << 9001 << std::endl;
                }
            })
        .run();

    return 0;
}