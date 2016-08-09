#include "db/backend/connector_interface.h"
#include <mutex>
#include <thread>
#include <unordered_map>

namespace db {

namespace {

std::mutex* get_connector_factory_lock() {
  static std::mutex connector_factory_lock;
  return &connector_factory_lock;
}

typedef std::unordered_map<std::string, ConnectorFactory*> ConnectorFactories;
ConnectorFactories* connector_factories() {
  static ConnectorFactories* factories = new ConnectorFactories;
  return factories;
}

} // namespace


// static
void ConnectorFactory::Register(const std::string& driver,
                                ConnectorFactory* factory) {
  std::lock_guard<std::mutex> l(*get_connector_factory_lock());
  if (!connector_factories()->insert({driver, factory}).second) {
    LOG(ERROR) << "Two server factories are begin registered under " << driver;
  }
}

// static
bool ConnectorFactory::GetFactory(const ConnectionInfo& info,
                                  ConnectorFactory** out_factory) {
  std::lock_guard<std::mutex> l(*get_connector_factory_lock());
  for (const auto& connector_factory : *connector_factories()) {
    if (connector_factory.second->AcceptsOptions(info)) {
      *out_factory = connector_factory.second;
      return true;
    }
  }
  return false;
}

bool NewConnector(const ConnectionInfo& info,
                  std::unique_ptr<ConnectorInterface>* out_connector) {
  ConnectorFactory* factory;
  bool r = ConnectorFactory::GetFactory(info, &factory);
  DCHECK(r);
  return factory->NewConnector(info, out_connector);
}

} // namespace base
