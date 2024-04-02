#include "db.hpp"

mongocxx::instance instance = {}; // This should be done only once.
mongocxx::uri uri("mongodb://localhost:27017");
mongocxx::client client(uri);