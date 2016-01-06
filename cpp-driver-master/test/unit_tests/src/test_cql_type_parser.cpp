/*
  Copyright (c) 2014-2015 DataStax

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE cassandra
#endif

#include "data_type_parser.hpp"
#include "metadata.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(cql_type_parser)

BOOST_AUTO_TEST_CASE(simple)
{
  cass::DataType::ConstPtr data_type;

  cass::NativeDataTypes native_types;
  native_types.init_cql_names();

  cass::KeyspaceMetadata keyspace("keyspace1");

  data_type = cass::DataTypeCqlNameParser::parse("ascii", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_ASCII);

  data_type = cass::DataTypeCqlNameParser::parse("bigint", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_BIGINT);

  data_type = cass::DataTypeCqlNameParser::parse("blob", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_BLOB);

  data_type = cass::DataTypeCqlNameParser::parse("boolean", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_BOOLEAN);

  data_type = cass::DataTypeCqlNameParser::parse("counter", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_COUNTER);

  data_type = cass::DataTypeCqlNameParser::parse("date", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_DATE);

  data_type = cass::DataTypeCqlNameParser::parse("decimal", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_DECIMAL);

  data_type = cass::DataTypeCqlNameParser::parse("double", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_DOUBLE);

  data_type = cass::DataTypeCqlNameParser::parse("float", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_FLOAT);

  data_type = cass::DataTypeCqlNameParser::parse("inet", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_INET);

  data_type = cass::DataTypeCqlNameParser::parse("int", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_INT);

  data_type = cass::DataTypeCqlNameParser::parse("smallint", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_SMALL_INT);

  data_type = cass::DataTypeCqlNameParser::parse("time", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_TIME);

  data_type = cass::DataTypeCqlNameParser::parse("timestamp", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_TIMESTAMP);

  data_type = cass::DataTypeCqlNameParser::parse("timeuuid", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_TIMEUUID);

  data_type = cass::DataTypeCqlNameParser::parse("tinyint", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_TINY_INT);

  data_type = cass::DataTypeCqlNameParser::parse("text", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_TEXT);

  data_type = cass::DataTypeCqlNameParser::parse("uuid", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_UUID);

  data_type = cass::DataTypeCqlNameParser::parse("varchar", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_VARCHAR);

  data_type = cass::DataTypeCqlNameParser::parse("varint", native_types, &keyspace);
  BOOST_CHECK_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_VARINT);
}

BOOST_AUTO_TEST_CASE(collections)
{
  cass::DataType::ConstPtr data_type;

  cass::NativeDataTypes native_types;
  native_types.init_cql_names();

  cass::KeyspaceMetadata keyspace("keyspace1");

  data_type = cass::DataTypeCqlNameParser::parse("list<int>", native_types, &keyspace);
  BOOST_REQUIRE_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_LIST);
  cass::CollectionType::ConstPtr list = static_cast<cass::CollectionType::ConstPtr>(data_type);
  BOOST_REQUIRE_EQUAL(list->types().size(), 1);
  BOOST_CHECK_EQUAL(list->types()[0]->value_type(), CASS_VALUE_TYPE_INT);

  data_type = cass::DataTypeCqlNameParser::parse("set<int>", native_types, &keyspace);
  BOOST_REQUIRE_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_SET);
  cass::CollectionType::ConstPtr set = static_cast<cass::CollectionType::ConstPtr>(data_type);
  BOOST_REQUIRE_EQUAL(set->types().size(), 1);
  BOOST_CHECK_EQUAL(set->types()[0]->value_type(), CASS_VALUE_TYPE_INT);

  data_type = cass::DataTypeCqlNameParser::parse("map<int, text>", native_types, &keyspace);
  BOOST_REQUIRE_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_MAP);
  cass::CollectionType::ConstPtr map = static_cast<cass::CollectionType::ConstPtr>(data_type);
  BOOST_REQUIRE_EQUAL(map->types().size(), 2);
  BOOST_CHECK_EQUAL(map->types()[0]->value_type(), CASS_VALUE_TYPE_INT);
  BOOST_CHECK_EQUAL(map->types()[1]->value_type(), CASS_VALUE_TYPE_TEXT);
}

BOOST_AUTO_TEST_CASE(tuple)
{
  cass::DataType::ConstPtr data_type;

  cass::NativeDataTypes native_types;
  native_types.init_cql_names();

  cass::KeyspaceMetadata keyspace("keyspace1");

  data_type = cass::DataTypeCqlNameParser::parse("tuple<int, bigint, text>", native_types, &keyspace);
  BOOST_REQUIRE_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_TUPLE);
  cass::CollectionType::ConstPtr tuple = static_cast<cass::CollectionType::ConstPtr>(data_type);
  BOOST_REQUIRE_EQUAL(tuple->types().size(), 3);
  BOOST_CHECK_EQUAL(tuple->types()[0]->value_type(), CASS_VALUE_TYPE_INT);
  BOOST_CHECK_EQUAL(tuple->types()[1]->value_type(), CASS_VALUE_TYPE_BIGINT);
  BOOST_CHECK_EQUAL(tuple->types()[2]->value_type(), CASS_VALUE_TYPE_TEXT);
}

BOOST_AUTO_TEST_CASE(udt)
{
  cass::DataType::ConstPtr data_type;

  cass::NativeDataTypes native_types;
  native_types.init_cql_names();

  cass::KeyspaceMetadata keyspace("keyspace1");

  BOOST_CHECK(keyspace.user_types().empty());

  data_type = cass::DataTypeCqlNameParser::parse("type1", native_types, &keyspace);

  BOOST_REQUIRE_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_UDT);
  cass::UserType::ConstPtr udt = static_cast<cass::UserType::ConstPtr>(data_type);
  BOOST_CHECK_EQUAL(udt->type_name(), "type1");
  BOOST_CHECK_EQUAL(udt->keyspace(), "keyspace1");

  BOOST_CHECK(!keyspace.user_types().empty());
}

BOOST_AUTO_TEST_CASE(frozen)
{
  cass::DataType::ConstPtr data_type;

  cass::NativeDataTypes native_types;
  native_types.init_cql_names();

  cass::KeyspaceMetadata keyspace("keyspace1");

  data_type = cass::DataTypeCqlNameParser::parse("frozen<list<int>>", native_types, &keyspace);
  BOOST_REQUIRE_EQUAL(data_type->value_type(), CASS_VALUE_TYPE_LIST);
  cass::CollectionType::ConstPtr list = static_cast<cass::CollectionType::ConstPtr>(data_type);
  BOOST_REQUIRE_EQUAL(list->types().size(), 1);
  BOOST_CHECK_EQUAL(list->types()[0]->value_type(), CASS_VALUE_TYPE_INT);
}

BOOST_AUTO_TEST_CASE(invalid)
{
  cass::DataType::ConstPtr data_type;

  cass::NativeDataTypes native_types;
  native_types.init_cql_names();

  cass::KeyspaceMetadata keyspace("keyspace1");

  // Invalid number of parameters
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("list<>", native_types, &keyspace));
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("set<>", native_types, &keyspace));
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("map<>", native_types, &keyspace));
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("tuple<>", native_types, &keyspace));

  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("list<int, int>", native_types, &keyspace));
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("set<int, int>", native_types, &keyspace));
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("map<int>", native_types, &keyspace));
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("map<int, int, int>", native_types, &keyspace));

  // Invalid brackets
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("list<", native_types, &keyspace));
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("list>", native_types, &keyspace));
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("<>", native_types, &keyspace));
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("<", native_types, &keyspace));
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse(">", native_types, &keyspace));

  // Empty
  BOOST_CHECK(!cass::DataTypeCqlNameParser::parse("", native_types, &keyspace));
}

BOOST_AUTO_TEST_SUITE_END()
