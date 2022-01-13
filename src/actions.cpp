#include <donations.hpp>
#include <leaderboard.hpp>

ACTION donations::setconfig(const config& cfg) {
  require_auth(get_self());
  //validate config
  check(cfg.round_length_sec > 0, "round_length_sec must be greater than 0");
  check(cfg.compound_step_sec > 0, "compound_step_sec must be greater than 0");
  check(cfg.round_length_sec > cfg.compound_step_sec, "round_length_sec must be greater then compound_step_sec");
  // check(cfg.start_time > NOW , "start_time must be in the future");
  //todo, review config checks.

  config_table _config(get_self(), get_self().value);
  _config.set(cfg, get_self());
}

ACTION donations::clrconfig() {
  require_auth(get_self());
  config_table _config(get_self(), get_self().value);
  check(_config.exists(), "Config table already removed");
  _config.remove();
}

ACTION donations::clrround(uint64_t& round_id) {
  require_auth(get_self());
  rounds_table _rounds(get_self(), get_self().value);
  auto r_itr = _rounds.find(round_id);
  check(r_itr != _rounds.end(), "Round with this id not found.");

  leaderboard_table _leaderboard(get_self(), round_id);
  check(_leaderboard.begin() == _leaderboard.end(), "Clear the leaderboard with round_id/scope " + to_string(round_id) + "first.");

  _rounds.erase(r_itr);
};

ACTION donations::claim(name& donator, uint64_t& round_id) {};

ACTION donations::rewardround(uint64_t& round_id) {
  Leaderboard lb(get_self());
  check(round_id != lb.round_id, "can't reward current round, wait until next round starts");
  rounds_table _rounds(get_self(), get_self().value);
  auto r_itr = _rounds.require_find(round_id, "round doesn't exist");
  // check(!r_itr->rewarded, "rewards already generated for this round"); // temp for debugging
  const auto reward_data = lb.generate_round_rewards(round_id, *r_itr);
  action({get_self(), "active"_n}, get_self(), name("rewardlog"), make_tuple(*r_itr, reward_data)).send();
  _rounds.modify(r_itr, get_self(), [&](rounds& row) {
    row.rewarded = true;
  });
};

ACTION donations::rewardlog(rounds& round_data, vector<rewards_data>& rewards_data) {
  require_auth(get_self());
}
