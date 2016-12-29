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
    static void reset() {
        clientPtr.reset(new mongocxx::client(mongocxx::uri{addr}));
        std::cout << "addr: " << addr << std::endl;
    }
private:
    static boost::thread_specific_ptr<mongocxx::client> clientPtr;
    static const std::string addr;
};
