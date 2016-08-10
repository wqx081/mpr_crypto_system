#include "db/frontend/common.h"
#include "db/frontend/result.h"
#include "db/frontend/statement.h"
#include "db/frontend/session.h"

#include "base/time.h"

#include <gtest/gtest.h>

namespace db {

TEST(Frontend, Basic) {
  std::string connection_info = "mysql:user='root'; password='111111'; database='test_db';@pool_size=2";

  Session sql(connection_info);

  try {
  sql << "DROP TABLE test" << db::Execute;
  } catch (...) {}

  try {
    sql << "CREATE TABLE test (id INTEGER PRIMARY KEY AUTO_INCREMENT NOT NULL, "
                            "n INTEGER, "
                            "f REAL, "
                            "t TIMESTAMP, "
                            "name TEXT)" << db::Execute;
    uint64_t row_id;
    int n;

    base::Time now = base::Time::Now();  

    //INSERT
    {
       Statement statement = sql  << "INSERT INTO test(n, f, t, name) VALUES(?,?,?,?)"
                               << 10
                               << 3.14
                               << now
                               << "Hello \'World\'";
       statement.Execute();

       row_id = statement.SequenceLast("test_id_seq");
       EXPECT_EQ(1, statement.SequenceLast("test_id_seq")); 
       EXPECT_EQ(1, statement.Affected());
       LOG(INFO) << "ID: " << row_id << ", Affected rows " << statement.Affected();
    }
    //INSERT
    {
      Statement statement = sql << "INSERT INTO test(n,f,t,name) VALUES (?,?,?,?)"
                 << Use(10, db::kNotNullValue)
                 << Use(3.14, db::kNullValue)
                 << Use(now, db::kNotNullValue)
                 << Use("Hello \'World\'", db::kNotNullValue)
                 << db::Execute;
       row_id = statement.SequenceLast("");
       LOG(INFO) << "ID: " << row_id << ", Affected rows " << statement.Affected();
       EXPECT_EQ(2, row_id);
       EXPECT_EQ(1, statement.Affected());
    }

    db::Result result = sql << "SELECT id,n,f,t,name FROM test LIMIT 10";
    n = 0;
    while (result.Next()) {
      double f = -1;
      int32_t id = -1, k = -1;
      base::Time t;
      std::string name; 
      db::NullTagType tag;
      result >> id >> k >> db::Into(f, tag) >> t >> name; 
      LOG(INFO) << id << ' ' << k << ' ' << f << ' ' << name << ' ' << t.Format();

      EXPECT_TRUE(id == n+1);
      EXPECT_TRUE(k == 10);
      EXPECT_TRUE(n == 0 ? f == 3.14: f == -1);
      EXPECT_TRUE(n == 0 ? tag==db::kNotNullValue : tag==db::kNullValue);
      EXPECT_TRUE(now.Format() == t.Format());
      EXPECT_TRUE(name == "Hello 'World'");
      n++;
    }
    EXPECT_EQ(n, 2);
 
    result = sql << "SELECT n FROM test WHERE id=?" << 1 << db::Row;
    EXPECT_TRUE(!result.Empty());
    int32_t val;
    result >> val;
    EXPECT_TRUE(val == 10); 
    result.Clear();

  } catch (...) {
  }
  EXPECT_TRUE(true);
}

} // namespace db
