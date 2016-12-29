#include "handlers.h"
#include "connection.h"

#include <bsoncxx/json.hpp>
#include <fastcgi2/data_buffer.h>
#include <iostream>
#include <chrono>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using ui32 = std::uint32_t;

const std::string SONGS_DBNAME = "test";
const std::string SONGS_COLLECTION_NAME = "songs";

template <typename Stream>
inline void docsToStream(Stream& stream, mongocxx::cursor& cursor) {
    bool first = true;
    for (auto&& doc : cursor) {
        if (!first) {
            stream << ", ";
            first = false;
        }
        stream << bsoncxx::to_json(doc);
    }
}

inline ui32 getLimit(fastcgi::Request* request, ui32 defaultValue = 10) {
    auto limitStr = request->hasArg("limit") ? request->getArg("limit") : "";
    return limitStr.empty() ? defaultValue : atoi(limitStr.c_str());
}

// GET /songs

void GetSongs::putLinks(fastcgi::RequestStream& stream) const {
    stream << "links: {"
           << "\"self\": {\"href\": \"/songs\"}"
           << ", \"best\": {\"href\": \"/songs/best\"}"
           << "\"new\": { \"href\": \"/songs/new\"}}";
}

void GetSongs::operator()(fastcgi::Request* request) const {
    auto limit = getLimit(request);

    auto& conn = Connection::get();

    auto songsCollection = conn[SONGS_DBNAME][SONGS_COLLECTION_NAME];

    mongocxx::options::find opts;
    opts.limit(limit);

    auto cursor = songsCollection.find({}, opts);
    
    fastcgi::RequestStream stream(request);
    request->setContentType("application/json");

    stream << "{";
    putLinks(stream);
    stream << ", \"songs\": [";
    docsToStream(stream, cursor);
    stream << "]}\n";

    request->setStatus(200);
}

// GET /songs/{id}

const std::string GetOneSong::PREFIX = "/songs/";

std::string GetOneSong::getId(fastcgi::Request* request) const {
    return request->getScriptName().substr(PREFIX.size());
}

void GetOneSong::operator()(fastcgi::Request* request) const {
    auto songUri = getId(request);

    auto& conn = Connection::get();
    auto songsCollection = conn[SONGS_DBNAME][SONGS_COLLECTION_NAME];

    auto query = document{} << "song_uri" << songUri << finalize;
    
    auto maybeResult = songsCollection.find_one(query.view());
    if (maybeResult) {
        request->setContentType("application/json");
        fastcgi::RequestStream stream(request);
        stream << bsoncxx::to_json(*maybeResult) << "\n";
        request->setStatus(200);
    } else {
        //not found
        request->setStatus(404);
    }
}

// GET /songs/new

void GetNewSongs::putLinks(fastcgi::RequestStream& stream) const {
    stream << "\"links\": {\"self\": {\"href\": \"/songs/new\"}}";
}

void GetNewSongs::operator()(fastcgi::Request* request) const {
    auto limit = getLimit(request);

    auto& conn = Connection::get();

    auto songsCollection = conn[SONGS_DBNAME][SONGS_COLLECTION_NAME];

    mongocxx::options::find opts;
    opts.limit(limit);
    auto sortOpt = document{} << "upload_date" << -1 << finalize;
    opts.sort(sortOpt.view());

    auto cursor = songsCollection.find({}, opts);

    fastcgi::RequestStream stream(request);
    request->setContentType("application/json");

    stream << "{";
    putLinks(stream);
    stream << ", \"songs\": [";
    docsToStream(stream, cursor);
    stream << "]}\n";

    request->setStatus(200);
}

// GET /songs/best

void GetBestSongs::putLinks(fastcgi::RequestStream& stream) const {
    stream << "\"links\": {\"self\": {\"href\": \"/songs/best\"}}";
}

void GetBestSongs::operator()(fastcgi::Request* request) const {
    auto limit = getLimit(request);

    auto& conn = Connection::get();

    auto songsCollection = conn[SONGS_DBNAME][SONGS_COLLECTION_NAME];

    mongocxx::options::find opts;
    opts.limit(limit);
    auto sortOpt = document{} << "likes" << -1 << finalize;
    opts.sort(sortOpt.view());

    auto cursor = songsCollection.find({}, opts);

    fastcgi::RequestStream stream(request);
    request->setContentType("application/json");

    stream << "{";
    putLinks(stream);
    stream << ", \"songs\": [";
    docsToStream(stream, cursor);
    stream << "]}\n";

    request->setStatus(200);
}

// POST /songs

std::time_t AddSong::timestamp() const {
    auto now = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(now);
}

void AddSong::operator()(fastcgi::Request* request) const {
    std::string body;
    fastcgi::DataBuffer bodyBuffer = request->requestBody();
    bodyBuffer.toString(body);

    bsoncxx::document::value song = bsoncxx::from_json(body);
    
    //add

    auto songUri = bsoncxx::oid().to_string();

    auto docValue = document{}
        << "song_uri" << songUri
        << "likes" << 0
        << "upload_date" << timestamp()
        << bsoncxx::builder::concatenate_doc{song.view()}
        << "links" << open_document
            << "self" << open_document
                << "href" << (std::string("/songs/") + songUri) << close_document
            << "download" << open_document
                << "href" << (std::string("/audiofiles/") + songUri) << close_document
            << "upload" << open_document
                << "href" << (std::string("/audiofiles/") + songUri) << close_document
            << "like" << open_document
                << "href" << (std::string("/songs/") + songUri + "/like") << close_document
        << close_document << finalize;

    auto& conn = Connection::get();
    auto songsCollection = conn[SONGS_DBNAME][SONGS_COLLECTION_NAME];

    auto maybeInserted = songsCollection.insert_one(docValue.view());
    if (!maybeInserted) {
        request->setStatus(500);
        return;
    }

    request->setContentType("application/json");
    fastcgi::RequestStream stream(request);
    stream << bsoncxx::to_json(docValue.view()) << "\n";
      
    request->setStatus(200);
}


// POST /songs/{id}/like

const std::string LikeSong::PREFIX = "/songs/";
const std::string LikeSong::SUFFIX = "/like";

std::string LikeSong::getId(fastcgi::Request* request) const {
    std::string url = request->getScriptName();
    url = url.substr(PREFIX.size());
    return url.substr(0, url.size() - SUFFIX.size());
}

void LikeSong::operator()(fastcgi::Request* request) const {
    auto songUri = getId(request);

    auto& conn = Connection::get();
    auto songsCollection = conn[SONGS_DBNAME][SONGS_COLLECTION_NAME];

    auto searchParams = document{} << "song_uri" << songUri << finalize;
    auto action = document{} << "$inc" << open_document << "likes" << 1 << close_document << finalize;

    auto maybeResult = songsCollection.update_one(searchParams.view(), action.view());
    if (!maybeResult) {
        request->setStatus(500);
        return;
    }

    request->setStatus(200);
}   
