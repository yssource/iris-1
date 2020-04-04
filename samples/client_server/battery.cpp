#include <iostream>
#include <iris/iris.hpp>
using namespace iris;

struct LedRequest {
  // Request body
  unsigned index;
  bool state;

  // Request Serialization
  template <typename Archive> void save(Archive &ar) const { ar(index, state); }
};

int main() {
  Component battery;
  auto client = battery.create_client(endpoints = {"tcp://127.0.0.1:5510"},
                                      timeout = 2500, retries = 3);
  battery.set_interval(
      period = 500, on_expiry = [&] {
        auto response = client.send(LedRequest{.index = 0, .state = true});
        std::cout << (response.get<bool>() ? "Success" : "Failed") << std::endl;
      });
  battery.start();
}
