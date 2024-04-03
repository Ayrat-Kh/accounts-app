#include "get-current-user.hpp"

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/json/src.hpp>

#include <iostream>
#include <string>

using namespace app::services;

std::variant<GoogleTokenInfo, std::exception> GoogleLoginImpl::GetGoogleUser(std::string_view idToken)
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    namespace ssl = net::ssl;
    using tcp = net::ip::tcp;

    try
    {
        net::io_context io_context;
        ssl::context ctx(ssl::context::sslv23_client);
        ctx.set_default_verify_paths();

        // Resolve the hostname
        tcp::resolver resolver(io_context);
        beast::ssl_stream<tcp::socket> stream(io_context, ctx);
        auto const endpoints = resolver.resolve("www.googleapis.com", "https");
        // Establish a connection
        net::connect(stream.next_layer(), endpoints);
        stream.handshake(ssl::stream_base::client);

        // Set up an HTTP POST request
        http::request<http::string_body> req;
        req.method(http::verb::post);
        req.target("/oauth2/v3/tokeninfo");                                      // Specify your API endpoint
        req.version(11);                                                         // HTTP/1.1
        req.set(http::field::host, "www.googleapis.com");                        // Set the host
        req.set(http::field::content_type, "application/x-www-form-urlencoded"); // Content type
        req.body() = "id_token=" + std::string(idToken);                         // Your POST data
        req.prepare_payload();

        // Send the request
        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        // Parse the JSON response
        boost::json::error_code ec;
        auto json_body = boost::json::parse(res.body(), ec);

        return GoogleTokenInfo{
            .alg = json_body.at("alg").as_string().c_str(),
            .at_hash = json_body.at("at_hash").as_string().c_str(),
            .aud = json_body.at("aud").as_string().c_str(),
            .azp = json_body.at("azp").as_string().c_str(),
            .email = json_body.at("email").as_string().c_str(),
            .email_verified = json_body.at("email_verified").as_string().c_str(),
            .exp = json_body.at("exp").as_string().c_str(),
            .family_name = json_body.at("family_name").as_string().c_str(),
            .given_name = json_body.at("given_name").as_string().c_str(),
            .iat = json_body.at("iat").as_string().c_str(),
            .iss = json_body.at("iss").as_string().c_str(),
            .kid = json_body.at("kid").as_string().c_str(),
            .name = json_body.at("name").as_string().c_str(),
            .picture = json_body.at("picture").as_string().c_str(),
            .sub = json_body.at("sub").as_string().c_str(),
            .typ = json_body.at("typ").as_string().c_str(),
        };
    }
    catch (const std::exception e)
    {
        std::cout << "Err" << e.what();
        return e;
    }
}