#include "connection.h"

boost::thread_specific_ptr<mongocxx::client> Connection::clientPtr;
unsigned int Connection::port = 27017;
