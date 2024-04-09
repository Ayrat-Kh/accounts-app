// #include <variant>

// #include <boost/json.hpp>
// #include <boost/describe.hpp>
#include <App.h>
// #include <boost/describe.hpp>
// #include <iostream>
// #include <string>

// #include "utils/mongoDocument.hpp"
#include "services/app-dependencies.hpp"
#include "auth/auth.handlers.hpp"
#include "users/users.handlers.hpp"

// #include <bsoncxx/builder/basic/document.hpp>
// #include <bsoncxx/json.hpp>
// #include <mongocxx/client.hpp>
// #include <mongocxx/instance.hpp>
// #include <mongocxx/stdx.hpp>
// #include <mongocxx/uri.hpp>

// #include <iostream>

// struct Nested
// {
//     int a;
// };
// BOOST_DESCRIBE_STRUCT(Nested, (), (a))

// struct Outer
// {
//     std::string name;
//     std::optional<std::string> name1;
//     int bbb;
//     std::vector<Nested> nested;
// };
// BOOST_DESCRIBE_STRUCT(Outer, (), (name, name1, bbb, nested))

int main(int argc, char *argv[])
{
    // using bsoncxx::builder::basic::kvp;
    // using bsoncxx::builder::basic::make_array;
    // using bsoncxx::builder::basic::make_document;

    // std::optional<int> a;

    //    a.has_value

    // bsoncxx::document::element element = {};

    // for (bsoncxx::v_noabi::array::element &elem : element.get_array().value)
    // {
    //     elem.
    //     // result.push_back(std::move(handleSerializer<typename T::value_type>(elem)));
    // }

    // boost::describe::for_each(boost::describe::describe_members<T, boost::describe::mod_public>(), [](auto m)
    // {
    // using MemberType = typename decltype(m)::type;
    // std::cout << "Member name: " << decltype(m)::name << ", type: " << typeid(MemberType).name() << '\n'; });

    // auto document = make_document(
    //     kvp("name", "foo"),
    //     kvp("bbb", 43),
    //     kvp("a", make_array(make_document(kvp("a", 110)))),
    //     kvp("f", make_document(kvp("a", 10))));

    // bsoncxx::builder::basic::document result = app::utils::toMongoDocument(Outer{.nested = {Nested{.a = 3}}});

    // std::cout << "bbb " << result.view()["nested"].get_array().value[0].get_document().value["a"].get_int32().value << std::endl;
    // std::cout << "bbb " << result.bbb << std::endl;
    // std::cout << "f.a " << result.a.size() << std::endl;
    // std::cout << "a[0] " << result.a[0].a << std::endl;

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
        .get(
            "/v1/users/:userId",
            app::users::handleGetUserById)
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