#include "get-current-user.hpp"

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>

using namespace app::services;

std::variant<GoogleTokenInfo, std::exception_ptr> GoogleLoginImpl::GetGoogleUser(std::string_view idToken)
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    namespace ssl = net::ssl;
    using tcp = net::ip::tcp;

    boost::json::value json_response;

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
        json_response = boost::json::parse(res.body(), ec);

        if (const auto error_string = json_response.at("error_description").if_string())
        {
            throw std::domain_error(error_string->c_str());
        }

        return GoogleTokenInfo{
            .alg = json_response.at("alg").as_string().c_str(),
            .at_hash = json_response.at("at_hash").as_string().c_str(),
            .aud = json_response.at("aud").as_string().c_str(),
            .azp = json_response.at("azp").as_string().c_str(),
            .email = json_response.at("email").as_string().c_str(),
            .email_verified = json_response.at("email_verified").as_string().c_str(),
            .exp = json_response.at("exp").as_string().c_str(),
            .family_name = json_response.at("family_name").as_string().c_str(),
            .given_name = json_response.at("given_name").as_string().c_str(),
            .iat = json_response.at("iat").as_string().c_str(),
            .iss = json_response.at("iss").as_string().c_str(),
            .kid = json_response.at("kid").as_string().c_str(),
            .name = json_response.at("name").as_string().c_str(),
            .picture = json_response.at("picture").as_string().c_str(),
            .sub = json_response.at("sub").as_string().c_str(),
            .typ = json_response.at("typ").as_string().c_str(),
        };
    }
    catch (const std::exception &e)
    {
        return std::current_exception();
    }
}