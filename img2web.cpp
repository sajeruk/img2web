#include "cassandra_db.h"
#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <iostream>
#include <sstream>
#include <string>

class ShowUsers : virtual public fastcgi::Component, virtual public fastcgi::Handler
{
private:
    CassandraDBClient client;
public:
    ShowUsers(fastcgi::ComponentContext *context) :
        fastcgi::Component(context)
    {

    }

    virtual void onLoad() {
        client.connect(ip_addr);
    }

    virtual void onUnload() {
    }

    virtual void handleRequest(fastcgi::Request* request, fastcgi::HandlerContext* context) {
        std::vector<std::string> v;
        if (client.getUsers(v) != CASS_OK) {
            request->setStatus(404);
            return;
        }
        request->setContentType("text/plain");
        std::stringbuf buf(JoinUsers(v));
        request->write(&buf);
    }
};

class PostImage : virtual public fastcgi::Component, virtual public fastcgi::Handler
{
private:
    CassandraDBClient client;
public:
    PostImage(fastcgi::ComponentContext *context) :
        fastcgi::Component(context)
    {

    }

    virtual void onLoad() {
        client.connect(ip_addr);
    }

    virtual void onUnload() {
    }

    virtual void handleRequest(fastcgi::Request* request, fastcgi::HandlerContext* context) {
        if (!request->hasArg("login") || !request->hasArg("image")) {
            request->setStatus(404);
            return;
        }
        RegisterSelf(client, request->getArg("login"));

        if (client.postImage(request->getArg("login"), request->getArg("image")) != CASS_OK) {
            request->setStatus(404);
            return;
        }
        request->setContentType("text/plain");
        std::stringbuf buffer("POST!");
        request->write(&buffer);
    }
};

class GetFeed : virtual public fastcgi::Component, virtual public fastcgi::Handler
{
private:
    CassandraDBClient client;
public:
    GetFeed(fastcgi::ComponentContext *context) :
        fastcgi::Component(context)
    {

    }

    virtual void onLoad() {
        client.connect(ip_addr);
    }

    virtual void onUnload() {
    }

    virtual void handleRequest(fastcgi::Request* request, fastcgi::HandlerContext* context) {
        if (!request->hasArg("login")) {
            request->setStatus(404);
            return;
        }

        size_t top = 10;
        if (request->hasArg("top")) {
            std::stringstream stream;
            try {
                stream << request->getArg("top");
                stream >> top;
            } catch (...) {
                top = 10;
                std::cerr << "cannot convert arg " << request->getArg("top")
                    << " defaulting to: " << top << std::endl;
            }
        }

        RegisterSelf(client, request->getArg("login"));
        std::vector<ImageDescription> desc;

        if (client.getFeed(request->getArg("login"), top, desc) != CASS_OK) {
            request->setStatus(404);
            return;
        }

        std::string res = "{ \"items\" : [";
        for (size_t i = 0; i + 1 < desc.size(); ++i) {
            const auto& item = desc[i];
            res += "{\"owner\":\"" + item.PostedBy + "\", \"image\": \"" +
               item.Image + "\"},";
        }
        if (!desc.empty()) {
            res += "{\"owner\":\"" + desc.back().PostedBy + "\", \"image\": \"" +
               desc.back().Image + "\"}";
        }
        res += " ] }";
        request->setContentType("application/json");
        std::stringbuf buffer(res);
        request->write(&buffer);
    }
};

class SubscribeTo : virtual public fastcgi::Component, virtual public fastcgi::Handler
{
private:
    CassandraDBClient client;
public:
    SubscribeTo(fastcgi::ComponentContext *context) :
        fastcgi::Component(context)
    {

    }

    virtual void onLoad() {
        client.connect(ip_addr);
    }

    virtual void onUnload() {
    }

    virtual void handleRequest(fastcgi::Request* request, fastcgi::HandlerContext* context) {
        if (!request->hasArg("login") || !request->hasArg("to")) {
            std::cerr << "args missing" << std::endl;
            return;
        }
        RegisterSelf(client, request->getArg("login"));

        if (client.execQuery("insert into img2web.subscriptions (login_from, "
            "login_to) VALUES ('" + request->getArg("login") +
            "','" + request->getArg("to") + "');") != CASS_OK)
        {
            request->setStatus(404);
            return;
        }

        request->setContentType("text/plain");
        std::stringbuf buffer("SUBSCRIBED!");
        request->write(&buffer);
    }
};

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("ShowUsersFactory", ShowUsers)
FCGIDAEMON_ADD_DEFAULT_FACTORY("SubscribeToFactory", SubscribeTo)
FCGIDAEMON_ADD_DEFAULT_FACTORY("GetFeedFactory", GetFeed)
FCGIDAEMON_ADD_DEFAULT_FACTORY("PostImageFactory", PostImage)
FCGIDAEMON_REGISTER_FACTORIES_END()
