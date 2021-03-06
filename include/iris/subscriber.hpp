#pragma once
#include <iostream>
#include <iris/component.hpp>
#include <iris/message.hpp>

namespace iris {

class Subscriber {
  friend Component;
  std::uint8_t id_;
  Component *component_;

  Subscriber(std::uint8_t id, Component *component)
      : id_(id), component_(component) {}

public:
  Subscriber() = default;

  void stop() { component_->stop_subscriber(id_); }
};

template <typename E, typename T, typename S>
inline Subscriber Component::create_subscriber(E &&endpoints, T &&timeout,
                                               S &&fn) {
  lock_t lock{subscribers_mutex_};
  auto s = std::make_unique<internal::SubscriberImpl>(
      subscriber_count_.load(), this, context_,
      std::forward<Endpoints>(Endpoints(endpoints)),
      std::forward<TimeoutMs>(TimeoutMs(timeout)),
      operation::SubscriberOperation{.fn = SubscriberFunction(fn).get()},
      executor_);
  subscribers_.insert(std::make_pair(subscriber_count_.load(), std::move(s)));
  return Subscriber(subscriber_count_++, this);
}

template <typename E, typename T, typename S>
inline internal::SubscriberImpl::SubscriberImpl(std::uint8_t id,
                                                Component *parent,
                                                zmq::context_t &context,
                                                E &&endpoints, T &&timeout,
                                                S &&fn, TaskSystem &executor)
    : id_(id), component_(parent), context_(context),
      endpoints_(std::move(endpoints)), fn_(fn), executor_(executor) {
  socket_ = std::make_unique<zmq::socket_t>(context, ZMQ_SUB);
  for (auto &e : endpoints_) {
    socket_->connect(e);
  }
  socket_->set(zmq::sockopt::subscribe, /*filter */ "");
  socket_->set(zmq::sockopt::rcvtimeo, timeout.get());
}

inline void internal::SubscriberImpl::recv() {
  while (!done_) {
    zmq::message_t message;
    auto ret = socket_->recv(message);
    if (ret.has_value()) {
      Message payload;
      payload.payload_ = std::move(message);
      payload.subscriber_id_ = id_;
      payload.component_ = component_;
      fn_.arg = payload;
      executor_.get().async_(fn_);
    }
  }
}

inline void internal::SubscriberImpl::start() {
  thread_ = std::thread(&SubscriberImpl::recv, this);
  started_ = true;
}

} // namespace iris