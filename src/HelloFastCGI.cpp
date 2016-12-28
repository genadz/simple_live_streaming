#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <mongocxx/instance.hpp>

#include <iostream>
#include "connection.h"
#include "handlers.h"

class HelloFastCGI : virtual public fastcgi::Component, virtual public fastcgi::Handler {

public:
    using HandlersMap = std::map<std::string, std::function<void (fastcgi::Request*)>>;


    HelloFastCGI(fastcgi::ComponentContext *context)
        : fastcgi::Component(context)
    {
        mongocxx::instance inst{};
        Connection::setPort(27017);

        handlers["/songs"] = GetSongs{};
    }
    virtual ~HelloFastCGI() {}

public:
    virtual void onLoad() {}
    virtual void onUnload() {}

    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context) {
        std::cout << request->getScriptName() << std::endl;
        auto it = handlers.find(request->getScriptName());
        if (it != handlers.end()) {
            it->second(request);
        } else {
            request->setStatus(404);
        }
    }

private:
    HandlersMap handlers;
};

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("hello_factory", HelloFastCGI)
FCGIDAEMON_REGISTER_FACTORIES_END()
