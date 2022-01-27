#pragma once
#define DEBUG
#define DAY_SEC 86400
#define _CONTRACT_NAME_ "donations"  //name of contract class
#if __INTELLISENSE__
#pragma diag_suppress 2486
#endif
#include <math.h>

#include <atomicassets.hpp>
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>

// #include "leaderboard.hpp"
#include "utils.hpp"
using namespace eosio;
using namespace std;

CONTRACT donations: public contract {
  using contract::contract;

 public:
  [[eosio::on_notify("atomicassets::logburnasset")]] void atomic_burn(name asset_owner,
                                                                      uint64_t asset_id,
                                                                      name collection_name,
                                                                      name schema_name,
                                                                      int32_t template_id,
                                                                      vector<asset> backed_tokens,
                                                                      atomicassets::ATTRIBUTE_MAP old_immutable_data,
                                                                      atomicassets::ATTRIBUTE_MAP old_mutable_data,
                                                                      name asset_ram_payer);
  [[eosio::on_notify("atomicassets::logtransfer")]] void atomic_transfer(name collection_name,
                                                                         name from,
                                                                         name to,
                                                                         vector<uint64_t> asset_ids,
                                                                         string memo);
  [[eosio::on_notify("atomicassets::logmint")]] void atomic_logmint(uint64_t asset_id,
                                                                    name authorized_minter,
                                                                    name collection_name,
                                                                    name schema_name,
                                                                    int32_t template_id,
                                                                    name new_asset_owner,
                                                                    atomicassets::ATTRIBUTE_MAP immutable_data,
                                                                    atomicassets::ATTRIBUTE_MAP mutable_data,
                                                                    vector<asset> backed_tokens,
                                                                    atomicassets::ATTRIBUTE_MAP immutable_template_data);
  [[eosio::on_notify("*::transfer")]] void token_deposit(name from, name to, asset quantity, string memo);
  eosio::time_point_sec NOW = eosio::time_point_sec(eosio::current_time_point());
  const permission_level active_auth = permission_level {get_self(), "active"_n};
  struct nft_config {
    asset mint_price_min;                     // minimum price to sell a bronze NFT (1st place)
    asset mint_price_increase_by_rank;        // as the leaderboard rank decreases, increase the NFT mint price by this amount.
    uint8_t max_bronze_mint_per_round;        // max number of bronze NFTs to mint per round
    uint8_t deposit_bronze_for_silver;        // deposit this many bronze NFTS into the contract to mint a silver
    uint8_t deposit_silver_for_gold;          // deposit this many silver NFTS into the contract to mint a gold
    uint8_t bonus_silver_per_bronze_claimed;  // after this many bronze NFTs are claimed by an account a silver will be claimed
    uint8_t bonus_gold_per_silver_claimed;    // after this many silver NFTs are claimed by an account a gold will be claimed
    name collection_name;                     // atomicassets collection
    name schema_name;                         // schema to use for minting rewards
    uint32_t bronze_template_id;
    uint32_t silver_template_id;
    uint32_t gold_template_id;
  };

  //table definitions
  TABLE config {
    uint64_t round_length_sec = 7 * 24 * 60 * 60;
    eosio::time_point_sec start_time;
    // the minimum first donation to be included in the leaderboard. this to prevent RAM attacks.
    eosio::asset minimum_donation = eosio::asset(1000, eosio::symbol(eosio::symbol_code("EOS"), 4));
    bool enabled = 0;  //disable leaderboard but still keep donations enabled

    float compound_decay_pct = 0.03;
    // uint32_t compound_step_sec = DAY_SEC;
    // uint32_t start_decay_after_sec = 0;
    uint64_t decay_step_sec = DAY_SEC;
    uint64_t start_decay_after_steps = 0;
    nft_config nft;
  };
  typedef eosio::singleton<"config"_n, config> config_table;

  TABLE leaderboard {
    eosio::name donator;
    uint64_t score;
    eosio::asset donated = eosio::asset(0, eosio::symbol(eosio::symbol_code("EOS"), 4));
    uint64_t times;
    eosio::time_point_sec last_donation;
    uint64_t primary_key() const { return donator.value; }
    uint64_t by_score() const { return static_cast<uint64_t>(UINT64_MAX - score); }
  };
  typedef eosio::multi_index<"leaderboard"_n, leaderboard,
                             eosio::indexed_by<"byscore"_n, eosio::const_mem_fun<leaderboard, uint64_t, &leaderboard::by_score>>>
    leaderboard_table;

  TABLE rounds {
    uint64_t id;                 //round_id = leaderboard scope
    eosio::asset total_donated;  //total amount donated in current round (not really needed in the contract)
    uint64_t total_score;        //total accumulated score of donations
    uint64_t donators;           //number of unique donators of current round
    eosio::time_point_sec start;
    bool rewarded;
    uint64_t primary_key() const { return id; }
  };
  typedef eosio::multi_index<"rounds"_n, rounds> rounds_table;

  TABLE claimed {
    name account;
    uint8_t bronze_unclaimed = 0;
    uint16_t bronze_claimed = 0;
    uint8_t silver_claimed = 0;
    uint8_t gold_claimed = 0;
    uint64_t primary_key() const { return account.value; }
  };
  typedef eosio::multi_index<"claimed"_n, claimed> claimed_table;

  TABLE balances {  // scope is user account name
    uint32_t template_id;
    uint16_t balance;
    uint64_t primary_key() const { return (uint64_t)template_id; }
  };
  typedef eosio::multi_index<"balances"_n, balances> balances_table;

  // functions
  void add_nfts(map<uint32_t, uint16_t> nft_deltas, donations::balances_table & balances_t);
  void sub_nfts(map<uint32_t, uint16_t> nft_deltas, donations::balances_table & balances_t);
  void nft_offer_swap(name receiver, map<uint32_t, uint16_t> nft_deltas, nft_config nft_conf);
  void mint(name receiver, uint32_t template_id, nft_config nft_conf);
  void burn(uint64_t asset_id);
  // actions
  ACTION setconfig(const config& cfg);
  ACTION clrconfig();
  ACTION clrround(uint64_t & round_id);
  ACTION rewardround(uint64_t & round_id);
  ACTION rewardlog(rounds & round_data, vector<rewards_data> & rewards_data);
  ACTION rmaccount(name & donator);
  ACTION claim(name & donator);

#if defined(DEBUG)
  ACTION simdonation(name donator, asset donation);
  ACTION clrleaderb(uint64_t scope);
  ACTION clrrounds();
  ACTION clrclaimed();
  ACTION clrbalances(name scope);
#endif
};
