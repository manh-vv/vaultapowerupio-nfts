#include <atomicassets.hpp>
#include <donations.hpp>
#include <leaderboard.hpp>

ACTION donations::setconfig(const config& cfg) {
  require_auth(get_self());
  // validate config
  check(cfg.round_length_sec > 0, "round_length_sec must be greater than 0");
  check(cfg.decay_step_sec > 0, "decay_step_sec must be greater than 0");
  check(cfg.round_length_sec > (cfg.decay_step_sec * cfg.start_decay_after_steps), "round_length_sec must be greater than the no decay period.");

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

ACTION donations::rewardround(uint64_t& round_id) {
  Leaderboard lb(get_self());
  check(round_id != lb.round_id, "can't reward current round, wait until round has ended");
  rounds_table _rounds(get_self(), get_self().value);
  auto r_itr = _rounds.require_find(round_id, "round doesn't exist");
  check(!r_itr->rewarded, "rewards already generated for this round");
  const auto reward_data = lb.generate_round_rewards(round_id, *r_itr);
  action(active_auth, get_self(), name("rewardlog"), make_tuple(*r_itr, reward_data)).send();
  _rounds.modify(r_itr, get_self(), [&](rounds& row) {
    row.rewarded = true;
  });
  auto conf = lb._conf.get();
  if(conf.nft.mint_price_min.amount > 1e4) {
    if(r_itr->total_donated.amount > 1e6) {
      conf.nft.mint_price_min += asset(1e3, symbol("EOS", 4));
    } else {
      conf.nft.mint_price_min -= asset(1e3, symbol("EOS", 4));
    }
    lb._conf.set(conf, get_self());
  }
};

ACTION donations::rewardlog(rounds& round_data, vector<rewards_data>& rewards_data) {
  require_auth(get_self());
};

ACTION donations::rmaccount(name& donator) {
  require_auth(get_self());
  claimed_table _accounts(get_self(), get_self().value);
  const auto donator_itr = _accounts.require_find(donator.value, "donator doesn't have an account");
  _accounts.erase(donator_itr);
};

ACTION donations::claim(name& donator) {
  donations::claimed_table _accounts(_self, _self.value);
  config_table _config(get_self(), get_self().value);
  const auto config = _config.get();
  const auto donator_itr = _accounts.require_find(donator.value, "donator doesn't have an account");
  const auto blank_data = atomicassets::ATTRIBUTE_MAP {};
  const vector<asset> tokens_to_back;
  auto claimer = *donator_itr;
  check(claimer.bronze_unclaimed > 0, "no nfts to claim");
  auto bronze_minted = 0;
  while(bronze_minted != claimer.bronze_unclaimed) {
    mint(claimer.account, config.nft.bronze_template_id, config.nft);
    bronze_minted++;
  }
  claimer.bronze_claimed += bronze_minted;
  claimer.bronze_unclaimed = 0;

  const auto total_mint_silver = claimer.bronze_claimed / config.nft.bonus_silver_per_bronze_claimed;
  if(total_mint_silver > claimer.silver_claimed) {
    auto mint_silver = total_mint_silver - claimer.silver_claimed;
    auto silver_minted = 0;
    while(silver_minted != mint_silver) {
      mint(claimer.account, config.nft.silver_template_id, config.nft);
      silver_minted++;
    }
    claimer.silver_claimed += silver_minted;
  }

  const auto total_mint_gold = claimer.silver_claimed / config.nft.bonus_gold_per_silver_claimed;
  if(total_mint_gold > claimer.gold_claimed) {
    auto mint_gold = total_mint_gold - claimer.gold_claimed;
    auto gold_minted = 0;
    while(gold_minted != mint_gold) {
      mint(claimer.account, config.nft.gold_template_id, config.nft);
      gold_minted++;
    }
    claimer.gold_claimed += gold_minted;
  }

  _accounts.modify(donator_itr, _self, [&](donations::claimed& row) {
    row = claimer;
  });
};
