#include <iostream>
#include <iris/component.hpp>
#include <iris/subscriber.hpp>

int main() {
  iris::component receiver;
  receiver.create_subscriber({"tcp://localhost:5555"}, [](std::string msg) {
    std::cout << "Received " << msg << "\n";
  });
  receiver.start();
}