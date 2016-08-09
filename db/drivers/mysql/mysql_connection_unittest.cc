#include "db/drivers/mysql/mysql_connection.h"
#include "db/common/connection_info.h"

#include <gtest/gtest.h>

namespace {

const db::ConnectionInfo info("mysql:host='localhost';user='root';password='111111';database='cookbook'");

} // namespace

namespace db {

TEST(MysqlConnection, Basic) {
  MysqlConnection connection(info);
  EXPECT_TRUE(true);
}

} // namespace db
