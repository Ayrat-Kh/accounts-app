#include <App.h>

#include "accounts/services/appDependencies.hpp"
#include "accounts/accounts/accounts.handlers.hpp"
#include "accounts/auth/auth.handlers.hpp"
#include "accounts/users/users.handlers.hpp"
#include "accounts/shared/env.hpp"

// #include <boost/describe.hpp>
// #include "accounts/utils/objectMapper.hpp"
// #include <string>
// #include <vector>
// #include <iostream>

// void *operator new(size_t size)
// {
//     std::cout << "New operator overloading " << size << std::endl;
//     void *p = malloc(size);
//     return p;
// }

// struct Type1Nested
// {
// public:
//     std::optional<std::string> name;

//     std::vector<int> nesInt;
// };
// BOOST_DESCRIBE_STRUCT(Type1Nested, (), (name, nesInt));

// struct Type1
// {
// public:
//     std::optional<std::string> name;

//     std::vector<Type1Nested> nesNes;

//     Type1Nested nested;
// };
// BOOST_DESCRIBE_STRUCT(Type1, (), (name, nested, nesNes));

// struct Type2Nested
// {
// public:
//     std::optional<std::string> name;

//     std::vector<int> nesInt;
// };
// BOOST_DESCRIBE_STRUCT(Type2Nested, (), (name, nesInt));

// struct Type2
// {
// public:
//     std::optional<std::string> name;

//     std::vector<Type2Nested> nesNes;

//     Type2Nested nested;
// };
// BOOST_DESCRIBE_STRUCT(Type2, (), (name, nesNes, nested));

int main(int argc, char *argv[])
{
    // std::vector<int>().
    // Type1 derived;
    // derived.name = "name 1";
    // derived.nested.name = "name 2";
    // derived.nesNes.push_back(
    //     Type1Nested{
    //         .name = "hello"});
    // derived.nested.nesInt.push_back(1432);

    //     std::cout << "start parser" << std::endl;

    // Type2 base = accounts::utils::remapObject<Type2>(std::move(derived));

    // std::cout << "end parser" << std::endl;

    // std::cout << "int " << base.nested.nesInt[0] << std::endl;
    // std::cout << "nes " << base.nesNes[0].name.value() << std::endl;

    // return 0;

    // ToDo config section - hardcoded for now. later read from env
    auto &env = accounts::getEnv();

    // dependency section
    ::accounts::AppDependencies::instance().init();

    // server section
    uWS::App()
        .get(
            "/health-check",
            [](uWS::HttpResponse<false> *res, auto *req)
            {
                res->end("Server is working fine !");
            })
        .post(
            "/login/google-auth/callback",
            accounts::handleGoogleLogin)
        .post(
            "/v1/accounts",
            accounts::handleAddAccount)
        .put(
            "/v1/accounts/:accountId",
            accounts::handleUpdateAccount)
        .get(
            "/v1/users/:userId",
            accounts::handleGetUserById)
        .get(
            "/v1/users/:userId/accounts",
            accounts::handleGetAccountsByUserId)
        .listen(
            env.port,
            [&env](auto *listen_socket)
            {
                if (listen_socket)
                {
                    std::cout << "Listening on port " << env.port << std::endl;
                }
            })
        .run();

    return 0;
}