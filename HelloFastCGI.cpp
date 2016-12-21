#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <iostream>

class HelloFastCGI : virtual public fastcgi::Component, virtual public fastcgi::Handler {

public:
    HelloFastCGI(fastcgi::ComponentContext *context)
        : fastcgi::Component(context)
    {}
    virtual ~HelloFastCGI() {}

public:
    virtual void onLoad() {}
    virtual void onUnload() {}

    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context) {
        request->setContentType("text/plain");
        std::string hello = "Hello, FastCGI!";
        request->write(hello.data(), hello.size());
    }

};

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("hello_factory", HelloFastCGI)
FCGIDAEMON_REGISTER_FACTORIES_END()
