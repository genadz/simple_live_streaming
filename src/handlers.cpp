#include "handlers.h"
#include "connection.h"

#include <bsoncxx/json.hpp>
#include <iostream>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

// GET /songs

const std::size_t GetSongs::defaultLimit = 100;

void GetSongs::putLinks(fastcgi::RequestStream& stream) const {
    stream << "links: {"
           << "\"self\": {\"href\": \"/songs\"}"
           << ", \"best\": {\"href\": \"/songs/best\"}"
           << "\"new\": { \"href\": \"/songs/new\"}}";
}

std::size_t GetSongs::getLimit(fastcgi::Request* request) const {
    auto limitStr = request->hasArg("limit") ? request->getArg("limit") : "";
    return limitStr.empty() ? defaultLimit : atoi(limitStr.c_str());
}

void GetSongs::operator()(fastcgi::Request* request) const {
    auto limit = getLimit(request);
    std::cout << "limit = " << limit << std::endl;

    auto& conn = Connection::get();

    auto songsCollection = conn["main"]["songs"];

    mongocxx::options::find opts;
    opts.limit(limit);

    auto cursor = songsCollection.find({}, opts);
    
    fastcgi::RequestStream stream(request);

    stream << "{";
    putLinks(stream);
    stream << ", \"songs\": [";

    bool first = true;
    for (auto&& doc : cursor) {
        if (!first)
            stream << ", ";
        stream << bsoncxx::to_json(doc);
        first = false;
    }

    stream << "]}\n";

    request->setContentType("application/json");
    request->setStatus(200);
}

// GET /songs/{id}

const std::string GetOneSong::PREFIX = "/songs/";

std::string GetOneSong::getId(fastcgi::Request* request) const {
    return request->getScriptName().substr(PREFIX.size());
}

void GetOneSong::operator()(fastcgi::Request* request) const {
    
}
