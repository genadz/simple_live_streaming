#include "connection.h"

boost::thread_specific_ptr<mongocxx::client> Connection::clientPtr;
const std::string Connection::addr = "mongodb://localhost:27017,localhost:27018,localhost:27019/?replicaSet=rs0";
