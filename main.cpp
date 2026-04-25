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
    if (incoming.side == Side::Buy)
      match_against(incoming, asks_);
    else
      match_against(incoming, bids_);
  }

  template<typename Map>
  void match_against(Order& incoming, Map& opposite) {
    while (incoming.quantity > 0 && !opposite.empty()) {
      auto it = opposite.begin();

      bool crosses = (incoming.side == Side::Buy) ? incoming.price >= it->first : incoming.price <= it->first;
      if (!crosses) break;

      auto& level = it->second;

      while (incoming.quantity > 0 && !level.empty()) {
        Order& resting = level.front();

        uint64_t fill = std::min(incoming.quantity, resting.quantity);

        Trade t;
        t.price = resting.price;
        t.quantity = fill;

        t.buy_order_id = (incoming.side == Side::Buy) ? incoming.id : resting.id;
        t.sell_order_id = (incoming.side == Side::Sell) ? incoming.id : resting.id;
        print_trade(t);

        incoming.quantity -= fill;
        resting.quantity -= fill;

        if (resting.quantity == 0)
        level.pop();
      }

      if (level.empty()) opposite.erase(it);
    }
  };

  void place(const Order& o) {
    if (o.side == Side::Buy)
      bids_[o.price].push(o);
    else
      asks_[o.price].push(o);
  }

  static void print_trade(const Trade& t) {
    std::cout << "TRADE"
      << "  buy=" << t.buy_order_id
      << "  sell=" << t.sell_order_id
      << "  px=" << t.price
      << "  qty=" << t.quantity << "\n";
  }
};

int main() {
  OrderBook book;

  // id, side, price, quantity
  book.insert({1, Side::Sell, 101.0, 10});
  book.insert({2, Side::Sell, 100.0, 5});
  book.insert({3, Side::Buy, 99.0, 8});

  // buy, limit 101, qty 12
  // matches 5 @ 100 against order 2 (fully fills order 2)
  // matches 7 @ 101 against order 1 (partial fill, order 1 has 3 remaining
  // nothing left to rest
  book.insert({4, Side::Buy, 101.0, 12});

  return 0;
}
