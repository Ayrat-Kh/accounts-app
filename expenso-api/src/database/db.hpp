#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

mongocxx::instance instance; // This should be done only once.
mongocxx::uri uri("mongodb://localhost:27017");
mongocxx::client client(uri);