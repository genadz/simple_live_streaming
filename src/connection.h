#include <mongocxx/client.hpp>
#include <boost/thread/tss.hpp>

#include <iostream>
#include <string>

class Connection {
public:
    static mongocxx::client& get() {
        if (!clientPtr.get())
            reset();
        return *clientPtr.get();
    }
    static void setPort(unsigned int p) { port = p; }
    static void reset() {
        auto addr = makeUri(port);
        clientPtr.reset(new mongocxx::client(mongocxx::uri{addr}));
        std::cout << "addr: " << addr << std::endl;
    }
private:
    static std::string makeUri(unsigned int port) {
        return std::string("mongodb://localhost:") + std::to_string(port);
    }
    static boost::thread_specific_ptr<mongocxx::client> clientPtr;
    static unsigned int port;
};
