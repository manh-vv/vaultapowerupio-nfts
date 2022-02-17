#include <atomicassets.hpp>
#include <donations.hpp>
#include <leaderboard.hpp>

#include "actions.cpp"

#if defined(DEBUG)
#include "debug_actions.cpp"
#endif

void donations::mint(name receiver, uint32_t template_id, nft_config nft_conf) {
  const auto blank_data = atomicassets::ATTRIBUTE_MAP {};
  const vector<asset> tokens_to_back;
  const auto data = make_tuple(get_self(), nft_conf.collection_name, nft_conf.schema_name, template_id, receiver, blank_data, blank_data, tokens_to_back);
  action(donations::active_auth, name("atomicassets"), name("mintasset"), data).send();
};

void donations::burn(uint64_t asset_id) {
  const auto data = make_tuple(get_self(), asset_id);
  action(active_auth, name("atomicassets"), name("burnasset"), data).send();
};

map<uint32_t, uint16_t> aggregate_nfts(vector<uint64_t> asset_ids, name owner) {
  map<uint32_t, uint16_t> template_counts;
  auto receiver_assets = atomicassets::assets_t(name("atomicassets"), owner.value);
  for(auto nft_id : asset_ids) {
    auto asset = *receiver_assets.require_find(nft_id, "powerup nfts error: can't find asset for this owner");
    auto existing = template_counts.find(asset.template_id);
    if(existing == template_counts.end()) {
      template_counts[asset.template_id] = 1;
    } else {
      template_counts[asset.template_id] += 1;
    }
  }
  return template_counts;
};

void donations::atomic_burn(name asset_owner,
                            uint64_t asset_id,
                            name collection_name,
                            name schema_name,
                            int32_t template_id,
                            vector<asset> backed_tokens,
                            atomicassets::ATTRIBUTE_MAP old_immutable_data,
                            atomicassets::ATTRIBUTE_MAP old_mutable_data,
                            name asset_ram_payer) {
  config_table _config(get_self(), get_self().value);
  const auto config = _config.get();
  if(collection_name != config.nft.collection_name) return;
  map<uint32_t, uint16_t> template_counts;
  template_counts[template_id] = 1;
  balances_table balances_t(get_self(), asset_owner.value);
  // if(asset_owner == get_self()) return;
  sub_nfts(template_counts, balances_t);
};

void donations::atomic_logmint(uint64_t asset_id,
                               name authorized_minter,
                               name collection_name,
                               name schema_name,
                               int32_t template_id,
                               name new_asset_owner,
                               atomicassets::ATTRIBUTE_MAP immutable_data,
                               atomicassets::ATTRIBUTE_MAP mutable_data,
                               vector<asset> backed_tokens,
                               atomicassets::ATTRIBUTE_MAP immutable_template_data) {
  config_table _config(get_self(), get_self().value);
  const auto config = _config.get();
  if(collection_name != config.nft.collection_name) return;
  donations::balances_table _balances(get_self(), new_asset_owner.value);
  map<uint32_t, uint16_t> template_counts;
  template_counts[template_id] = 1;
  add_nfts(template_counts, _balances);
}

void donations::token_deposit(name from, name to, eosio::asset quantity, std::string memo) {
  if(from == get_self() || from == name("eosio") || from == name("eosio.rex") || from == name("eosio.stake")) return;
  if(to != get_self()) return;
  const name tkn_contract = get_first_receiver();
  if(tkn_contract != name("eosio.token")) return;
  if(quantity.symbol.code() != symbol_code("EOS")) return;
  Leaderboard lb(get_self());
  lb.addScore(from, quantity);
}

void donations::stake_nft(name owner, uint32_t template_id, uint64_t asset_id) {
  donations::staked_table staked_t(get_self(), owner.value);
  staked_table::const_iterator staked_itr = staked_t.find((uint64_t)template_id);
  check(staked_itr == staked_t.end(), "can't stake an already staked NFT template");
  eosio::time_point_sec now = eosio::time_point_sec(eosio::current_time_point());
  staked_t.emplace(get_self(), [&](staked& row) {
    row.locked_until = time_point_sec(now.sec_since_epoch() + (86400 * 30 * 3));  // three month lockup
    row.template_id = template_id;
    row.asset_id = asset_id;
  });
}

void donations::atomic_transfer(name collection_name,
                                name from,
                                name to,
                                vector<uint64_t> asset_ids,
                                string memo) {
  const name tkn_contract = get_first_receiver();
  if(tkn_contract != name("atomicassets")) return;
  config_table _config(get_self(), get_self().value);
  const auto config = _config.get();
  if(collection_name != config.nft.collection_name) return;
  auto template_counts = aggregate_nfts(asset_ids, to);
  donations::balances_table _balances1(get_self(), to.value);
  add_nfts(template_counts, _balances1);
  donations::balances_table _balances2(get_self(), from.value);
  sub_nfts(template_counts, _balances2);
  if(to == get_self()) {
    if(memo == string("stake")) {
      check(asset_ids.size() == 1, "can only stake one NFT per transaction");
      stake_nft(from, template_counts.begin()->first, asset_ids[0]);
    } else {
      nft_offer_swap(from, template_counts, config.nft);
      for(auto asset_id : asset_ids) {
        burn(asset_id);
      }
    }
  }
}

void donations::add_nfts(map<uint32_t, uint16_t> nft_deltas, donations::balances_table& balances_t) {
  for(auto nft_bal : nft_deltas) {
    auto balances_itr = balances_t.find(nft_bal.first);
    if(balances_itr == balances_t.end()) {
      balances_t.emplace(get_self(), [&](balances& row) {
        row.template_id = nft_bal.first;
        row.balance = nft_bal.second;
      });
    } else {
      balances_t.modify(balances_itr, get_self(), [&](balances& row) {
        auto row_before = row;
        row.balance += nft_bal.second;
        check(row_before.balance < row.balance, "overflow error, you can't hold more of this asset");
      });
    }
  }
}

void donations::sub_nfts(map<uint32_t, uint16_t> nft_deltas, donations::balances_table& balances_t) {
  for(auto nft_bal : nft_deltas) {
    auto balances_itr = balances_t.require_find((uint64_t)nft_bal.first, "nft accounting error, contact eospowerup.io admin");
    auto new_bal = balances_itr->balance - nft_bal.second;
    check(new_bal < balances_itr->balance, "subtract error");
    if(new_bal == 0) {
      balances_t.erase(balances_itr);
    } else {
      balances_t.modify(balances_itr, get_self(), [&](balances& row) {
        row.balance = new_bal;
      });
    }
  }
}

void donations::nft_offer_swap(name receiver, map<uint32_t, uint16_t> nft_deltas, nft_config nft_conf) {
  check(nft_deltas.size() == 1, "all nfts must be of the same type");
  uint32_t template_id = nft_deltas.begin()->first;  // key
  auto quantity = nft_deltas.begin()->second;
  const auto blank_data = atomicassets::ATTRIBUTE_MAP {};
  const vector<asset> tokens_to_back;
  if(template_id == nft_conf.bronze_template_id) {
    check(quantity == nft_conf.deposit_bronze_for_silver, "You need to transfer exactly " + to_string(nft_conf.deposit_bronze_for_silver) + " bronze nfts to mint a silver one");
    mint(receiver, nft_conf.silver_template_id, nft_conf);
  } else if(template_id == nft_conf.silver_template_id) {
    check(quantity == nft_conf.deposit_silver_for_gold, "You need to transfer exactly " + to_string(nft_conf.deposit_silver_for_gold) + " bronze nfts to mint a silver one");
    mint(receiver, nft_conf.gold_template_id, nft_conf);
  } else if(template_id == nft_conf.gold_template_id) {
    check(false, "Golden NFTs can't be burned at this time");
  } else
    check(false, "can't deposit that NFT");
}
