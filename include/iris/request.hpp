#pragma once
#include <iris/cereal/archives/json.hpp>
#include <iris/cppzmq/zmq.hpp>
#include <sstream>

namespace iris {

namespace internal {
class ServerImpl;
class AsyncServerImpl;
} // namespace internal

class Request {
  zmq::message_t payload_;
  class Component *component_;
  std::uint8_t server_id_;
  bool async_;
  friend class TaskSystem;
  friend class internal::ServerImpl;
  friend class internal::AsyncServerImpl;

public:
  Request() {}

  Request(const Request &rhs) {
    payload_.copy(const_cast<Request &>(rhs).payload_);
    component_ = rhs.component_;
    server_id_ = rhs.server_id_;
    async_ = rhs.async_;
  }

  Request &operator=(Request rhs) {
    std::swap(payload_, rhs.payload_);
    std::swap(component_, rhs.component_);
    std::swap(server_id_, rhs.server_id_);
    std::swap(async_, rhs.async_);
    return *this;
  }

  template <typename T> T get() {
    T result;
    std::stringstream stream;
    stream.write(reinterpret_cast<const char *>(payload_.data()),
                 payload_.size());
    {
      cereal::JSONInputArchive archive(stream);
      archive(result);
    }
    return result;
  }
};

}; // namespace iris