#include "db/backend/db_statement.h"
#include <list>

namespace db {

DBStatement::DBStatement() : cache_(nullptr) {}

void DBStatement::SetCache(DBStatementsCache* cache) {
  cache_ = cache;
}

// static
void DBStatement::Dispose(DBStatement* self) {
  if (!self) {
    return;
  }

  DBStatementsCache* cache = self->cache_;
  self->cache_ = nullptr;
  if (cache) {
    cache->Put(self);
  } else {
    delete self;
  }
}

// Cache
struct DBStatementsCache::Data {

  Data() : size(0), max_size(0) {}

  struct Entry;
  using DBStatementsType = std::map<std::string, Entry>;
  using LRUType = std::list<DBStatementsType::iterator>;
  struct Entry {
    DBStatementPtr db_statement;
    LRUType::iterator lru_ptr;
  };

  DBStatementsType db_statements;
  LRUType lru;
  size_t size;
  size_t max_size;

  void Insert(DBStatementPtr statement) {
    DBStatementsType::iterator it;
    if ((it = db_statements.find(statement->SqlQuery())) 
        != db_statements.end()) {
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

      std::pair<DBStatementsType::iterator, bool> ret = 
	      db_statements.insert(std::make_pair(statement->SqlQuery(),
				                  Entry()));
      it = ret.first;
      it->second.db_statement = statement;
      lru.push_front(it);
      it->second.lru_ptr = lru.begin();
      size++;
    }
  }

  DBStatementPtr Fetch(const std::string& query) {
    DBStatementPtr result;
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


DBStatementsCache::DBStatementsCache() {}
void DBStatementsCache::SetSize(size_t n) {
  if (n != 0 && !IsActive()) {
    data_.reset(new Data());
    data_->max_size = n;
  }
}

void DBStatementsCache::Put(DBStatement* statement) {
  if (!IsActive()) {
    delete statement;
  }
  DBStatementPtr p(statement);
  p->Reset();
  data_->Insert(p);
}

DBStatementPtr DBStatementsCache::Fetch(const std::string& query) {
  if (!IsActive()) {
    return 0;
  }
  return data_->Fetch(query);
}

void DBStatementsCache::Clear() {
  data_->Clear();
}

bool DBStatementsCache::IsActive() {
  return data_.get() != nullptr;
}

DBStatementsCache::~DBStatementsCache() {}

} // namespace db
