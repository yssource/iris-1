#pragma once
#include <iostream>
#include <iris/component.hpp>

namespace iris {

class Client {
  friend Component;
  std::uint8_t id_;
  Component *component_;

  Client(std::uint8_t id, Component *component)
      : id_(id), component_(component) {}

public:
  Client() = default;

  template <typename R> iris::Response send(R &&request) {
    return component_->request(id_, std::forward<R>(request));
  }
};

template <typename E, typename T, typename R>
inline Client Component::create_client(E &&endpoints, T &&timeout,
                                       R &&retries) {
  lock_t lock{clients_mutex_};
  auto p = std::make_unique<internal::ClientImpl>(
      context_, std::forward<Endpoints>(Endpoints(endpoints)),
      std::forward<TimeoutMs>(TimeoutMs(timeout)),
      std::forward<Retries>(Retries(retries)), executor_);
  clients_.insert(std::make_pair(client_count_.load(), std::move(p)));
  return Client(client_count_++, this);
}

} // namespace iris