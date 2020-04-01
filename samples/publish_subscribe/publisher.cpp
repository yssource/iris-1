#include <iostream>
#include <iris/component.hpp>
#include <iris/publisher.hpp>
#include <iris/timer.hpp>

int main() {
  iris::component sender;
  auto p = sender.create_publisher({"tcp://*:5555"});
  sender.set_interval(50, [&p] { p.send("Hello, World!"); });
  sender.start();
}