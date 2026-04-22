#include <cstdint>
#include <map>
#include <queue>
#include <iostream>

enum class Side { Buy, Sell };

struct Order {
  uint64_t id;
  Side side;
  double price;
  uint64_t quantity;
};

struct Trade {
  uint64_t buy_order_id;
  uint64_t sell_order_id;
  double price;
  uint64_t quantity;
};

class OrderBook{
public:
  void insert(Order order) {
    match(order);
    if (order.quantity > 0)
      place(order);
  };
private:
  std::map<double, std::queue<Order>, std::greater<double>> bids_;
  std::map<double, std::queue<Order>, std::less<double>> asks_;

  void match(Order& incoming) {

  };

  void place(const Order& o) {

  };
};
