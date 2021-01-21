#ifndef SRC_MAINARB_STRATEGY_H_
#define SRC_MAINARB_STRATEGY_H_

#include <unordered_map>
#include <cmath>
#include <vector>

#include <string>
#include <iostream>
#include <deque>
#include <memory>
#include <tuple>

#include <libconfig.h++>

#include "struct/market_snapshot.h"
#include "struct/strategy_status.h"
#include "struct/strategy_mode.h"
#include "struct/order.h"
#include "struct/command.h"
#include "struct/exchange_info.h"
#include "struct/order_status.h"
#include "util/time_controller.h"
#include "util/zmq_sender.hpp"
#include "util/dater.h"
#include "util/history_worker.h"
#include "util/contract_worker.h"
#include "util/common_tools.h"
#include "core/base_strategy.h"

class Strategy : public BaseStrategy {
 public:
  explicit Strategy(const libconfig::Setting & param_setting, std::unordered_map<std::string, std::vector<BaseStrategy*> >*ticker_strat_map, ZmqSender<MarketSnapshot>* uisender, ZmqSender<Order>* ordersender, TimeController* tc, ContractWorker* cw, HistoryWorker* hw, const std::string & date, StrategyMode::Enum mode = StrategyMode::Real, std::ofstream* exchange_file = nullptr);
  ~Strategy();

  void Start() override;
  void Stop() override;

  // void Clear() override;
  void HandleCommand(const Command& shot) override;
  // void UpdateTicker() override;
 private:
  bool FillStratConfig(const libconfig::Setting& param_setting);
  void RunningSetup(std::unordered_map<std::string, std::vector<BaseStrategy*> >*ticker_strat_map, ZmqSender<MarketSnapshot>* uisender, ZmqSender<Order>* ordersender);
  void DoOperationAfterUpdateData(const MarketSnapshot& shot) override;
  void DoOperationAfterUpdatePos(Order* o, const ExchangeInfo& info) override;
  void DoOperationAfterFilled(Order* o, const ExchangeInfo& info) override;
  void DoOperationAfterCancelled(Order* o) override;
  void ModerateOrders(const std::string & contract) override;

  bool Ready() override;
  void Pause() override;
  void Resume() override;
  void Run() override;
  void Train() override;
  void Flatting() override;

  double OrderPrice(const std::string & contract, OrderSide::Enum side, bool control_price) override;

  bool OpenLogic();

  void CalParams();
  std::tuple<double, double> CalMeanStd(const std::vector<double> & v, int head, int num);

  double GetPairMid();

  void ForceFlat() override;

  bool Spread_Good();

  bool IsAlign();

  void HandleTestOrder(Order *o);

  char order_ref[MAX_ORDERREF_SIZE];
  std::string main_ticker;
  std::string hedge_ticker;
  int max_pos;
  double min_price_move;

  int cancel_limit;
  std::unordered_map<std::string, double> mid_map, bid_map, ask_map;
  double up_diff;
  double down_diff;
  double range_width;
  double mean;
  std::vector<double> mids;
  int current_pos;
  double min_profit;
  int train_samples;
  double min_range;
  double increment;
  std::string date;
  double spread_threshold;
  int closed_size;
  double last_valid_mid;
  int max_close_try;
  double current_spread;
  bool is_started;
  bool no_close_today;
  int max_round;
  int close_round;
  int split_num;
  int sample_head;
  int sample_tail;
  std::ofstream* exchange_file;
  double target_hedge_price;
};

#endif  // SRC_MAINARB_STRATEGY_H_
