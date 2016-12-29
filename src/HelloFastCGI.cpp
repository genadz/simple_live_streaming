#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <mongocxx/instance.hpp>

#include <iostream>
#include <boost/algorithm/string/predicate.hpp>

#include "connection.h"
#include "handlers.h"

inline std::string mapUrl(const std::string& toMap) {
    if (boost::starts_with(toMap, "/songs/") && (toMap != "/songs/new") && (toMap != "/songs/best")) {
        if (boost::ends_with(toMap, "/like")) {
            return "/songs/{id}/like";
        }
        return "/songs/{id}";
    }
    return toMap;
}

class HelloFastCGI : virtual public fastcgi::Component, virtual public fastcgi::Handler {

public:
    using HandlersMap = std::map<std::string, std::function<void (fastcgi::Request*)>>;


    HelloFastCGI(fastcgi::ComponentContext *context)
        : fastcgi::Component(context)
    {
        mongocxx::instance inst{};
        Connection::setPort(27017);

        getHandlers["/songs"] = GetSongs{};
        getHandlers["/songs/new"] = GetNewSongs{};
        getHandlers["/songs/best"] = GetBestSongs{};
        getHandlers["/songs/{id}"] = GetOneSong{};

        postHandlers["/songs"] = AddSong{};
        postHandlers["/songs/{id}/like"] = LikeSong{};
    }
    virtual ~HelloFastCGI() {}

public:
    virtual void onLoad() {}
    virtual void onUnload() {}

    virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context) {
        //std::cout << request->getRequestMethod() << " " << request->getScriptName() << std::endl;
        const std::string method = request->getRequestMethod();
        if (method == "GET") {
            handleOne(request, getHandlers);
        } else if (method == "POST") {
            handleOne(request, postHandlers);
        } else {
            //not supported
            request->setStatus(405);
        }
    }

private:
    void handleOne(fastcgi::Request *request, HandlersMap& handlers) {
        auto mapped = mapUrl(request->getScriptName());
        auto it = handlers.find(mapped);
        if (it != handlers.end()) {
            it->second(request);
        } else {
            //not found
            request->setStatus(404);
        }
    }

    HandlersMap getHandlers;
    HandlersMap postHandlers;
};

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("hello_factory", HelloFastCGI)
FCGIDAEMON_REGISTER_FACTORIES_END()
