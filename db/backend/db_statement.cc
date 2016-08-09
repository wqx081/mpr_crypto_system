#include "db/backend/db_statement.h"
#include <list>

namespace db {

DBStatement::DBStatement() 
  : cache_(nullptr) {}

DBStatement::~DBStatement() {
  DBStatementCache* cache = cache_;
  cache_ = nullptr;
  if (cache) {
    cache_->Put(this);
  }
}

struct DBStatementCache::Data {
  struct Entry;
  using DBStatementsType = std::map<std::string, Entry>;
  using LruType = std::list<DBStatementsType::iterator>;
  struct Entry {
    scoped_ref_ptr<DBStatement> db_statement;  
    LruType::iterator lru_ptr;
  };

  DBStatementsType db_statements;
  LruType lru;
  size_t size;
  size_t max_size;

  Data() : size(0), max_size(0) {}

  void Insert(scoped_ref_ptr<DBStatement> statement) {
    DBStatementsType::iterator it;
    if ((it = db_statements.find(statement->SqlQuery())) != db_statements.end()) {
      it->second.db_statement = statement;
      lru.erase(it->second.lru_ptr);
      lru.push_front(it);
      it->second.lru_ptr = lru.begin();
    } else {
      if (size > 0 && size >= max_size) {
        db_statements.erase(lru.back());
        lru.pop_back();
        size--;
      }
      auto item = db_statements.insert(std::make_pair(statement->SqlQuery(), Entry()));
      it = item.first;
      it->second.db_statement = statement;
      lru.push_front(it);
      it->second.lru_ptr = lru.begin();
      size++;
    }
  }
  
  scoped_ref_ptr<DBStatement> Fetch(const std::string& query) {
    scoped_ref_ptr<DBStatement> result;
    DBStatementsType::iterator it = db_statements.find(query);
    if (it == db_statements.end()) {
      return result;
    }
    result = it->second.db_statement;
    lru.erase(it->second.lru_ptr);
    db_statements.erase(it);
    size--;
    return result;
  }

  void Clear() {
    lru.clear();
    db_statements.clear();
    size = 0;
  }

};

DBStatementCache::DBStatementCache() {}

void DBStatementCache::SetSize(size_t n) {
  if (n != 0 && !IsActive()) {
    data_.reset(new Data());
    data_->max_size = n;
  }  
}

void DBStatementCache::Put(scoped_ref_ptr<DBStatement> statement) {
  if (!IsActive()) {
    statement = nullptr;
  }
  statement->Reset();
  data_->Insert(statement);
}

scoped_ref_ptr<DBStatement> DBStatementCache::Fetch(const std::string& query) {
  if (!IsActive()) {
    return nullptr;
  }
  return data_->Fetch(query);
}

void DBStatementCache::Clear() {
  data_->Clear();
}

DBStatementCache::~DBStatementCache() {}

bool DBStatementCache::IsActive() {
  return data_.get() != nullptr;
}

} // namespace db
