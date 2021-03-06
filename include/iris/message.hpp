#pragma once
#include <iris/cppzmq/zmq.hpp>

namespace iris {

namespace internal {
class SubscriberImpl;
}

class Message {
  zmq::message_t payload_;
  class Component *component_;
  std::uint8_t subscriber_id_;
  friend class internal::SubscriberImpl;

public:
  Message() {}

  Message(const Message &rhs) {
    payload_.copy(const_cast<Message &>(rhs).payload_);
    component_ = rhs.component_;
    subscriber_id_ = rhs.subscriber_id_;
  }

  Message &operator=(Message rhs) {
    std::swap(payload_, rhs.payload_);
    std::swap(component_, rhs.component_);
    std::swap(subscriber_id_, rhs.subscriber_id_);
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
    return std::move(result);
  }
};

}; // namespace iris