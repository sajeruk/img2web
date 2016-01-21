#pragma once

#include "cpp-driver-master/include/cassandra.h"
#include <vector>
#include <string>
#include <sstream>
#include <memory>

typedef std::unique_ptr<CassCluster, decltype(&cass_cluster_free)> cluster_ptr;
typedef std::unique_ptr<CassSession, decltype(&cass_session_close)> session_ptr;
typedef std::unique_ptr<CassFuture, decltype(&cass_future_free)> future_ptr;
typedef std::unique_ptr<CassStatement, decltype(&cass_statement_free)> statement_ptr;
typedef std::unique_ptr<CassIterator, decltype(&cass_iterator_free)> iter_ptr;
typedef std::unique_ptr<const CassResult, decltype(&cass_result_free)> result_ptr;

extern const char* ip_addr;

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
    cluster_ptr cluster;
    session_ptr session;
public:
    inline CassSession* getSession() { return session.get(); }
    inline CassCluster* getCluster() { return cluster.get(); }

    CassandraDBClient()
        : cluster(nullptr, cass_cluster_free)
        , session(nullptr, cass_session_close)
    {}
    CassError connect(const std::string nodes);

    inline CassError execQuery(const std::string& query, result_ptr* results = nullptr);

    CassError getSubscriptionsCommon(const std::string& login,
        bool to, std::vector<std::string>& v);

    CassError getUsers(std::vector<std::string>& v);

    CassError getFeed(const std::string& login, size_t limit, std::vector<ImageDescription>& desc);

    CassError postImage(const std::string& login, const std::string& base64);
};

void RegisterSelf(CassandraDBClient& client, const std::string& login);
std::string JoinUsers(const std::vector<std::string>& v);
