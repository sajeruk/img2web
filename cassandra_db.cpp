#include "cassandra_db.h"
#include <iostream>

const char* ip_addr = "127.0.0.1";

CassError CassandraDBClient::connect(const std::string nodes) {
    CassError rc = CASS_OK;
    std::cerr << "Connecting to " << nodes << std::endl;
    cluster = std::move(cluster_ptr(cass_cluster_new(), cass_cluster_free));
    session = std::move(session_ptr(cass_session_new(), cass_session_close));
    cass_cluster_set_contact_points(cluster.get(), ip_addr);
    future_ptr connect_future(cass_session_connect(session.get(), cluster.get()), cass_future_free);
    cass_future_wait(connect_future.get());
    rc = cass_future_error_code(connect_future.get());

    if ( rc == CASS_OK ) {
        std::cerr << "Connected." << std::endl;
    } else {
        std::cerr << cass_error_desc(rc) << std::endl;
    }

    return rc;
}

CassError CassandraDBClient::execQuery(const std::string& query, result_ptr* results/* = nullptr*/) {
    CassError rc = CASS_OK;

    statement_ptr statement(
        cass_statement_new_n(query.c_str(), query.size(), 0),
        cass_statement_free);

    std::cerr << "Executing " << query << std::endl;
    future_ptr result_future(
        cass_session_execute(session.get(), statement.get()),
        cass_future_free);

    cass_future_wait(result_future.get());

    rc = cass_future_error_code(result_future.get());

    if (rc == CASS_OK)
    {
        std::cerr << "Statement " << query << " executed successully." << std::endl;
        if (results != nullptr)
        {
            *results = std::move(result_ptr(cass_future_get_result(result_future.get()), cass_result_free));
        }
    }
    else
    {
        std::cerr << cass_error_desc(rc) << std::endl;
    }

    return rc;
}

CassError CassandraDBClient::getSubscriptionsCommon(const std::string& login,
    bool to, std::vector<std::string>& v)
{
    // to - true - who is subscribed to login
    // else to whom login is subscribed
    CassError rc = CASS_OK;

    const std::string first = to ? "login_from" : "login_to";
    const std::string scd = to ? "login_to" : "login_from";
    std::string query = "SELECT " + first + " FROM img2web.subscriptions"
      "  WHERE " + scd  + " = '" + login + "';";


    result_ptr result(nullptr, cass_result_free);
    rc = execQuery(query, &result);
    if (rc != CASS_OK) {
        return rc;
    }

    iter_ptr rows(cass_iterator_from_result(result.get()), cass_iterator_free);

    while (cass_iterator_next(rows.get())) {
        const CassRow* row = cass_iterator_get_row(rows.get());
        const CassValue* value = cass_row_get_column_by_name(row, first.c_str());

        const char* str;
        size_t len;
        cass_value_get_string(value, &str, &len);
        v.push_back(std::string(str, len));
    }

    return rc;
}

CassError CassandraDBClient::getUsers(std::vector<std::string>& v) {
    result_ptr result(nullptr, cass_result_free);
    CassError rc = execQuery("SELECT login FROM img2web.users;", &result);
    if (rc != CASS_OK) {
        return rc;
    }
    iter_ptr rows(cass_iterator_from_result(result.get()), cass_iterator_free);
    while (cass_iterator_next(rows.get())) {
        const CassRow* row = cass_iterator_get_row(rows.get());
        const CassValue* value = cass_row_get_column_by_name(row, "login");

        const char* str;
        size_t len;
        cass_value_get_string(value, &str, &len);
        v.push_back(std::string(str, len));
    }

    return rc;
}

CassError CassandraDBClient::getFeed(const std::string& login, size_t limit, std::vector<ImageDescription>& desc) {
    // first take all subscriptions
    result_ptr result(nullptr, cass_result_free);
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

    iter_ptr rows(cass_iterator_from_result(result.get()), cass_iterator_free);
    while (cass_iterator_next(rows.get())) {
        const CassRow* row = cass_iterator_get_row(rows.get());
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

    return rc;
}

CassError CassandraDBClient::postImage(const std::string& login, const std::string& base64) {
    //const std::string img = base64_decode(base64);
    const std::string query = "INSERT INTO img2web.images (login, ts, img) VALUES ('"
        + login + "', now(),'" + base64 +  "');";
    return execQuery(query);
}

void RegisterSelf(CassandraDBClient& client, const std::string& login) {
    client.execQuery("insert into img2web.users (login) VALUES ('" + login + "');");
    client.execQuery("insert into img2web.subscriptions (login_from, "
        "login_to) VALUES ('" + login + "','" + login + "');");
}

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
