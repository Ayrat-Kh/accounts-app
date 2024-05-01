#include <iostream>

#include "env.hpp"

const accounts::EnvironmentVar &accounts::getEnv()
{
    static EnvironmentVar variables = {};

    if (variables.mongoUrl.empty())
    {
        boost::process::environment e = boost::this_process::environment();

        if (e["ACCOUNTS_DB_URL"].empty())
        {
            const char *errorMessage = R"(Please set "ACCOUNTS_DB_URL")";
            std::cerr << errorMessage << std::endl;
            throw new std::domain_error(errorMessage);
        }

        if (e["ACCOUNTS_JWT_KEY"].empty())
        {
            const char *errorMessage = R"(Please set "ACCOUNTS_JWT_KEY")";
            std::cerr << errorMessage << std::endl;
            throw new std::domain_error(errorMessage);
        }

        if (e["ACCOUNTS_GEOCODE_API_KEY"].empty())
        {
            const char *errorMessage = R"(Please set "ACCOUNTS_GEOCODE_API_KEY")";
            std::cerr << errorMessage << std::endl;
            throw new std::domain_error(errorMessage);
        }

        variables.geoCodeApiKey = e["ACCOUNTS_GEOCODE_API_KEY"].to_string();
        variables.mongoUrl = e["ACCOUNTS_DB_URL"].to_string();
        variables.jwtKey = e["ACCOUNTS_JWT_KEY"].to_string();
        variables.port = e["ACCOUNTS_PORT"].empty() ? 3000 : atoi(e["ACCOUNTS_PORT"].to_string().c_str());
        variables.dbName = e["ACCOUNTS_DB"].empty() ? "accounts" : e["ACCOUNTS_DB"].to_string().c_str();
    }

    return variables;
}
