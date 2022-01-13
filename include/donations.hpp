#pragma once
#define DEBUG
#define DAY_SEC 86400
#define _CONTRACT_NAME_ "donations"  //name of contract class
#if __INTELLISENSE__
#pragma diag_suppress 2486
#endif
#include <math.h>

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
  eosio::time_point_sec NOW = eosio::time_point_sec(eosio::current_time_point());

  //table defenitions
  TABLE config {
    uint64_t round_length_sec = 7 * 24 * 60 * 60;
    eosio::time_point_sec start_time;
    //the minimum first donation to be included in the leaderboard. this to prevent RAM attacks.
    eosio::asset minimum_donation = eosio::asset(1000, eosio::symbol(eosio::symbol_code("EOS"), 4));
    bool enabled = 0;  //disable leaderboard but still keep donations enabled

    float compound_decay_pct = 0.03;
    uint32_t compound_step_sec = DAY_SEC;
    uint32_t start_decay_after_sec = 0;
    asset mint_price_min = eosio::asset(10000, eosio::symbol(eosio::symbol_code("EOS"), 4));
    asset mint_price_increase_by_rank = eosio::asset(5000, eosio::symbol(eosio::symbol_code("EOS"), 4));
    uint8_t max_mint_per_round = 7;
    uint8_t bronze_to_silver = 20;  // after this many bronze NFTs are claimed by an account a silver can be claimed
    uint8_t silver_to_gold = 10;    // after this many silver NFTs are claimed by an account a gold can be claimed
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

  TABLE accounts {
    name account;
    uint8_t bronze_unclaimed = 0;
    uint16_t bronze_claimed = 0;
    uint8_t silver_claimed = 0;
    uint8_t gold_claimed = 0;
    uint64_t primary_key() const { return account.value; }
  };
  typedef eosio::multi_index<"accounts"_n, accounts> accounts_table;

  //actions
  ACTION setconfig(const config& cfg);
  ACTION clrconfig();
  ACTION claim(name & donator, uint64_t & round_id);
  ACTION clrround(uint64_t & round_id);
  ACTION rewardround(uint64_t & round_id);
  ACTION rewardlog(rounds & round_data, vector<rewards_data> & rewards_data);
  // ACTION∏∏
#if defined(DEBUG)
  ACTION simdonation(name donator, asset donation);
  ACTION clrleaderb(uint64_t scope);
  ACTION clrrounds();
  ACTION clraccounts();
#endif

  //there need to be an account table to hold extra data
  //ie to record NFT transfers

  //  struct [[eosio::table, eosio::contract(_CONTRACT_NAME_)]] accountdata{
  //      eosio::name account;
  //      bool is_donator;
  //      uint64_t redeem_points;
  //      uint16_t tier; //adjust tier according to NFT transfer notifications
  //      //more data from account can be stored here.

  //      uint64_t primary_key() const { return account.value; }
  //  };
  //  typedef eosio::multi_index<"accountdata"_n, accountdata> accountdata_table;
};
