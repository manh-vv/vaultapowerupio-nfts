template <typename T>
void cleanTable(name code, uint64_t account, const uint32_t batchSize) {
  T db(code, account);
  uint32_t counter = 0;
  auto itr = db.begin();
  while(itr != db.end() && counter++ < batchSize) {
    itr = db.erase(itr);
  }
}

ACTION donations::simdonation(name donator, asset donation) {
  // require_auth(get_self());
  Leaderboard lb(get_self());
  lb.addScore(donator, donation);
}

ACTION donations::clrleaderb(uint64_t scope) {
  //require_auth(get_self());
  cleanTable<donations::leaderboard_table>(get_self(), scope, 100);
};
