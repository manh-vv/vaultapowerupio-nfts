#pragma once
using namespace eosio;
static uint16_t u16add(const uint16_t x, const uint16_t y) {
  const uint16_t z = x + y;
  eosio::check(z >= x, "safemath-add-overflow");
  return z;
}
static uint8_t u8add(const uint8_t x, const uint8_t y) {
  const uint8_t z = x + y;
  eosio::check(z >= x, "safemath-add-overflow");
  return z;
}
struct rewards_data {
  name donator;
  eosio::asset donated;
  uint64_t score;
  uint8_t rank;
  uint8_t bronze_nfts_awarded;
};
