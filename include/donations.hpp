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

using namespace eosio;
using namespace std;

CONTRACT donations: public contract {
  using contract::contract;

 public:
  eosio::time_point_sec NOW = eosio::time_point_sec(eosio::current_time_point());

  //table defenitions
  TABLE config {
    uint64_t round_length_sec = 7 * 24 * 60 * 60;
    uint16_t max_rounds = 4;
    eosio::time_point_sec start_time;
    //the minimum first donation to be included in the leaderboard. this to prevent RAM attacks.
    eosio::asset minimum_donation = eosio::asset(5000, eosio::symbol(eosio::symbol_code("EOS"), 4));
    bool enabled = 0;  //disable leaderboard but still keep donations enabled

    double compound_decay_pct = 0.05;
    uint64_t compound_step_sec = DAY_SEC;
    uint64_t start_decay_after_sec = 0;

    //add configs for decreasing time bonus
    //add config for volume bonus  donated/totaldonated
  };
  typedef eosio::singleton<"config"_n, config> config_table;

  TABLE leaderboard {
    eosio::name donator;
    uint64_t score;
    eosio::asset donated;
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
    uint64_t total_score;        //number of unique donators of current round
    uint64_t donators;           //number of unique donators of current round
    eosio::time_point_sec start;
    uint64_t primary_key() const { return id; }
  };
  typedef eosio::multi_index<"rounds"_n, rounds> rounds_table;

  //actions
  ACTION setconfig(const config& cfg);
  ACTION clrconfig();
  ACTION claim(name & donator, uint64_t & round_id);
  ACTION clrround(uint64_t & round_id);

#if defined(DEBUG)
  ACTION simdonation(name donator, asset donation);
  ACTION clrleaderb(uint64_t scope);
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
