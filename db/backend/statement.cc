#include "db/backend/statement.h"

#include <map>
#include <list>

namespace db {
namespace backend {

Statement::Statement()
  : cache_(nullptr) {}

Statement::~Statement() {}

void Statement::SetCache(StatementsCache* cache) {
  cache_ = cache;
}

void Statement::Dispose(Statement* statement) {
  if (!statement) {
    return;
  }
  StatementsCache* cache = statement->cache_;
  statement->cache_ = nullptr;
  if (cache) {
    cache->Put(statement);
  } else {
    delete statement;
  }
}

//// StatementsCache
//
struct StatementsCache::Data {
  struct Entry;
  typedef std::map<std::string, Entry> StatementsType;
  typedef std::list<StatementsType::iterator> LruType;
  struct Entry {
    SimpleRefPtr<Statement> statement;
    LruType::iterator lru_iterator;
  };
  StatementsType statements;
  LruType lru;
  size_t size;
  size_t max_size;

  Data(): size(0), max_size(0) {}  

  void Insert(SimpleRefPtr<Statement> statement) {
    StatementsType::iterator it;
    if ((it = statements.find(statement->SqlQuery())) != statements.end()) {
      it->second.statement = statement;
      lru.erase(it->second.lru_iterator);
      lru.push_front(it);
      it->second.lru_iterator = lru.begin();
    } else {
      if (size > 0 && size >= max_size) {
        statements.erase(lru.back());
        lru.pop_back();
        size--;
      }
      std::pair<StatementsType::iterator, bool> res =
        statements.insert(std::make_pair(statement->SqlQuery(), Entry()));
      it = res.first;
      it->second.statement = statement;
      lru.push_front(it);
      it->second.lru_iterator = lru.begin();
      size++;
    }
  }

  SimpleRefPtr<Statement> Fetch(const std::string& query) {
    SimpleRefPtr<Statement> statement;
    StatementsType::iterator it = statements.find(query);  
    if (it == statements.end()) {
      return statement;
    }
    statement = it->second.statement;
    lru.erase(it->second.lru_iterator);
    statements.erase(it);
    size--;
    return statement;
  }

  void Clear() {
    lru.clear();
    statements.clear();
    size = 0;
  }
};

StatementsCache::StatementsCache() {
}

void StatementsCache::SetSize(size_t n) {
  if (n != 0 && !IsActive()) {
    data_.reset(new Data());
    data_->max_size = n;
  }
}

void StatementsCache::Put(Statement* statement) {
  if (!IsActive()) {
    delete statement;
  }
  SimpleRefPtr<Statement> ptr(statement);
  ptr->Reset();
  data_->Insert(ptr);
}

SimpleRefPtr<Statement> StatementsCache::Fetch(const std::string& query) {
  if (!IsActive()) {
    return nullptr;
  }
  return data_->Fetch(query);
}

void StatementsCache::Clear() {
  data_->Clear();
}

StatementsCache::~StatementsCache() {}

bool StatementsCache::IsActive() {
  return data_.get() != nullptr;
}

} // namespace backend
} // namespace db
