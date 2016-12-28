#include <fastcgi2/request.h>
#include <fastcgi2/stream.h>

struct IHandler {
    virtual void operator()(fastcgi::Request *request) const = 0;
    std::string getUid(fastcgi::Request* request) const {
        return request->hasArg("uid") ? request->getArg("uid") : "";
    }
};

struct GetSongs : public IHandler {
    void operator()(fastcgi::Request* request) const override;
private:
    std::size_t getLimit(fastcgi::Request* request) const;
    void putLinks(fastcgi::RequestStream& stream ) const;
    static const std::size_t defaultLimit;
};

struct GetOneSong : public IHandler {
    void operator()(fastcgi::Request* request) const override;
private:
    std::string getId() const;
    static const std::string PREFIX;
};

struct GetNewSongs : public IHandler {
    void operator()(fastcgi::Request* request) const override;
private:
    static const std::size_t defaultLimit;
};

struct GetBestSongs : public IHandler {
    void operator()(fastcgi::Request* request) const override;
private:
    static const std::size_t defaultLimit;
};
