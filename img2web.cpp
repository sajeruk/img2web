#include "base64.h"
#include "cpp-driver-master/include/cassandra.h"

#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>

#include <iostream>
#include <sstream>

std::string JoinUsers(const std::vector<std::string>& v) {
    std::string result = "(";
    for (size_t i = 0; i + 1 < v.size(); ++i) {
        result += "'" + v[i] + "',";
    }
    if (!v.empty()) {
        result += "'" + v.back() + "'";
    }
    result += ")";
    return result;
}

struct ImageDescription {
    std::string PostedBy;
    std::string Image;

    ImageDescription(const std::string& posted_by, const std::string& img)
        : PostedBy(posted_by)
        , Image(img)
    {}
};

class CassandraDBClient {
private:
    CassCluster* cluster = nullptr;
    CassSession* session = nullptr;
public:
    inline CassSession* getSession() { return session; }
    inline CassCluster* getCluster() { return cluster; }

    CassError connect(const std::string nodes) {
        CassError rc = CASS_OK;
        std::cerr << "Connecting to " << nodes << std::endl;
        cluster = cass_cluster_new();
        session = cass_session_new();
        CassFuture* connect_future = nullptr;
        cass_cluster_set_contact_points(cluster, "127.0.0.1");
        connect_future = cass_session_connect(session, cluster);
        cass_future_wait(connect_future);
        rc = cass_future_error_code(connect_future);

        if ( rc == CASS_OK )
        {
            std::cerr << "Connected." << std::endl;
        }
        else
        {
            std::cerr << cass_error_desc(rc) << std::endl;
            return rc;
        }

        cass_future_free(connect_future);
        return rc;
    }

    inline CassError execQuery(const std::string& query, const CassResult** results = nullptr) {
        CassError rc = CASS_OK;
        CassFuture* result_future = nullptr;

        CassStatement* statement = cass_statement_new_n(query.c_str(), query.size(), 0);

        std::cerr << "Executing " << query << std::endl;
        result_future = cass_session_execute(session, statement);
        cass_future_wait(result_future);

        rc = cass_future_error_code(result_future);

        if (rc == CASS_OK)
        {
            std::cerr << "Statement " << query << " executed successully." << std::endl;
            if (results != nullptr)
            {
                *results = cass_future_get_result(result_future);
            }
        }
        else
        {
            std::cerr << cass_error_desc(rc) << std::endl;
            cass_statement_free(statement);
            cass_future_free(result_future);
            return rc;
        }

        cass_statement_free(statement);
        cass_future_free(result_future);

        return rc;
    }

    CassError getSubscriptionsCommon(const std::string& login,
        bool to, std::vector<std::string>& v)
    {
        // to - true - who is subscribed to login
        // else to whom login is subscribed
        CassError rc = CASS_OK;
        CassFuture* future = nullptr;

        const std::string first = to ? "login_from" : "login_to";
        const std::string scd = to ? "login_to" : "login_from";
        std::string query = "SELECT " + first + " FROM img2web.subscriptions"
          "  WHERE " + scd  + " = '" + login + "';";

        const CassResult* result;
        rc = execQuery(query, &result);
        if (rc != CASS_OK) {
            return rc;
        }

        CassIterator* rows = cass_iterator_from_result(result);

        while (cass_iterator_next(rows)) {
            const CassRow* row = cass_iterator_get_row(rows);
            const CassValue* value = cass_row_get_column_by_name(row, first.c_str());

            const char* str;
            size_t len;
            cass_value_get_string(value, &str, &len);
            v.push_back(std::string(str, len));
        }

        cass_result_free(result);
        cass_iterator_free(rows);

        return rc;
    }

    CassError getUsers(std::vector<std::string>& v) {
        const CassResult* result;
        CassError rc = execQuery("SELECT login FROM img2web.users;", &result);
        if (rc != CASS_OK) {
            return rc;
        }
        CassIterator* rows = cass_iterator_from_result(result);
        while (cass_iterator_next(rows)) {
            const CassRow* row = cass_iterator_get_row(rows);
            const CassValue* value = cass_row_get_column_by_name(row, "login");

            const char* str;
            size_t len;
            cass_value_get_string(value, &str, &len);
            v.push_back(std::string(str, len));
        }

        cass_result_free(result);
        cass_iterator_free(rows);

        return rc;
    }

    CassError getFeed(const std::string& login, size_t limit, std::vector<ImageDescription>& desc) {
        // first take all subscriptions
        const CassResult* result;
        std::vector<std::string> subscriptions;
        CassError rc = getSubscriptionsCommon(login, false, subscriptions);
        if (rc != CASS_OK) {
            return rc;
        }
        const std::string sbs = JoinUsers(subscriptions);
        std::stringstream stream;
        stream << "SELECT * from img2web.images WHERE login IN " << sbs
            << " ORDER BY ts DESC LIMIT " << limit << ";";
        rc = execQuery(stream.str(), &result);

        if (rc != CASS_OK) {
            return rc;
        }

        CassIterator* rows = cass_iterator_from_result(result);
        while (cass_iterator_next(rows)) {
            const CassRow* row = cass_iterator_get_row(rows);
            const CassValue* posted_by = cass_row_get_column_by_name(row, "login");
            const CassValue* image = cass_row_get_column_by_name(row, "img");

            const char* name;
            size_t name_len;
            cass_value_get_string(posted_by, &name, &name_len);

            const char* img;
            size_t img_len;
            cass_value_get_string(image, &img, &img_len);
            desc.push_back(ImageDescription(std::string(name, name_len), std::string(img, img_len)));
        }

        cass_result_free(result);
        cass_iterator_free(rows);

        return rc;
    }

    CassError postImage(const std::string& login, const std::string& base64) {
        //const std::string img = base64_decode(base64);
        const std::string query = "INSERT INTO img2web.images (login, ts, img) VALUES ('"
            + login + "', now(),'" + base64 +  "');";
        return execQuery(query);
    }

    void close() {
        std::cerr << "Closing down cluster connection." << std::endl;
        if (session) {
            cass_session_close(session);
        }
        if (cluster) {
            cass_cluster_free(cluster);
        }
    }
};

void RegisterSelf(CassandraDBClient& client, const std::string& login) {
    client.execQuery("insert into img2web.users (login) VALUES ('" + login + "');");
    client.execQuery("insert into img2web.subscriptions (login_from, "
        "login_to) VALUES ('" + login + "','" + login + "');");
}

class ShowUsers : virtual public fastcgi::Component, virtual public fastcgi::Handler
{
private:
    CassandraDBClient client;
public:
    ShowUsers(fastcgi::ComponentContext *context) :
        fastcgi::Component(context)
    {

    }

    virtual void onLoad()
    {
        client.connect("127.0.0.1");
    }

    virtual void onUnload()
    {
        client.close();
    }

    virtual void handleRequest(fastcgi::Request* request, fastcgi::HandlerContext* context)
    {
        std::vector<std::string> v;
        client.getUsers(v);
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

    virtual void onLoad()
    {
        client.connect("127.0.0.1");
    }

    virtual void onUnload()
    {
        client.close();
    }

    virtual void handleRequest(fastcgi::Request* request, fastcgi::HandlerContext* context)
    {
        if (!request->hasArg("login") || !request->hasArg("image")) {
            return;
        }
        RegisterSelf(client, request->getArg("login"));

        client.postImage(request->getArg("login"), request->getArg("image"));
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

    virtual void onLoad()
    {
        client.connect("127.0.0.1");
    }

    virtual void onUnload()
    {
        client.close();
    }

    virtual void handleRequest(fastcgi::Request* request, fastcgi::HandlerContext* context)
    {
        if (!request->hasArg("login")) {
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

        client.getFeed(request->getArg("login"), top, desc);

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

    virtual void onLoad()
    {
        client.connect("127.0.0.1");
    }

    virtual void onUnload()
    {
        client.close();
    }

    virtual void handleRequest(fastcgi::Request* request, fastcgi::HandlerContext* context)
    {
        if (!request->hasArg("login") || !request->hasArg("to")) {
            std::cerr << "args missing" << std::endl;
            return;
        }
        RegisterSelf(client, request->getArg("login"));

        client.execQuery("insert into img2web.subscriptions (login_from, "
            "login_to) VALUES ('" + request->getArg("login") +
            "','" + request->getArg("to") + "');");

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
