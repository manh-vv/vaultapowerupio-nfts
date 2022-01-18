#pragma once
using namespace eosio;

struct rewards_data {
  name donator;
  eosio::asset donated;
  uint64_t score;
  uint8_t rank;
  uint8_t bronze_nfts_awarded;
};
