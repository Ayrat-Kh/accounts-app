#include <variant>

#include <boost/json.hpp>

#include "auth/auth.handlers.hpp"
#include "auth/auth.utils.hpp"
#include "users/users.repository.hpp"
#include "services/mongo-access.hpp"
#include "services/google-login.service.hpp"
#include "services/jwt.service.hpp"
#include "utils/read-request-json.hpp"
#include "utils/error.hpp"

#include <App.h>

#include "services/app-dependencies.hpp"

int main(int argc, char *argv[])
{
    // ToDo config section - hardcoded for now. later read from env
    int32_t appPort = 3000;

    // dependency section
    app::services::AppDependencies::instance().init();

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
            app::auth::handleGoogleLogin)
        .listen(
            appPort,
            [appPort](auto *listen_socket)
            {
                if (listen_socket)
                {
                    std::cout << "Listening on port " << appPort << std::endl;
                }
            })
        .run();

    return 0;
}