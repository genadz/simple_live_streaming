#include <fastcgi2/request.h>
#include <fastcgi2/stream.h>

#include <ctime>

struct IHandler {
    virtual void operator()(fastcgi::Request *request) const = 0;
    std::string getUid(fastcgi::Request* request) const {
        return request->hasArg("uid") ? request->getArg("uid") : "";
    }
};

// GET

struct GetSongs : public IHandler {
    void operator()(fastcgi::Request* request) const override;
private:
    void putLinks(fastcgi::RequestStream& stream ) const;
};

struct GetOneSong : public IHandler {
    void operator()(fastcgi::Request* request) const override;
private:
    std::string getId(fastcgi::Request* request) const;
    static const std::string PREFIX;
};

struct GetNewSongs : public IHandler {
    void operator()(fastcgi::Request* request) const override;
private:
    void putLinks(fastcgi::RequestStream& stream) const;
};

struct GetBestSongs : public IHandler {
    void operator()(fastcgi::Request* request) const override;
private:
    void putLinks(fastcgi::RequestStream& stream) const;
};


// POST

struct AddSong : public IHandler {
    void operator()(fastcgi::Request* request) const override;
private:
    std::time_t timestamp() const;
};

struct LikeSong : public IHandler {
    void operator()(fastcgi::Request* request) const override;
private:
    std::string getId(fastcgi::Request* request) const;
    static const std::string PREFIX;
    static const std::string SUFFIX;
};
