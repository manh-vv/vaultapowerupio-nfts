const conf = require('./eosioConfig')
const env = require('../.env.js')
const utils = require('@deltalabs/eos-utils')
const { api, tapos, doAction } = require('./lib/eosjs')(env.keys[env.defaultChain], conf.endpoints[env.defaultChain][0])
// api.de
// api.deserialize(, 'name')
const contractAccount = conf.accountName[env.defaultChain]
var watchAccountSample = require('./lib/sample_watchaccount')

const methods = {

  //leaderboard
  async simdonation(donator, donation) {
    // donator="blabla";
    // donation="1.0000 EOS"
    await doAction('simdonation', { donator, donation })
  },
  async clrleaderb(scope) {

    await doAction('clrleaderb', { scope })
  },
  async clearLeaderboards() {
    const scopes = (await api.rpc.get_table_by_scope({ code: contractAccount, table: "leaderboard" })).rows.map(el => el.scope)
    console.log('leaderboards:', scopes.length);
    for (let scope of scopes) {
      scope = utils.convertName2Value(scope).toInt()
      console.log(scope);
      await doAction('clrleaderb', { scope })
    }
  },
  async setconfig(cfg) {
    cfg = {
      round_length_sec: 60 * 2,
      minimum_donation: "1.0000 EOS",
      enabled: 1,
      compound_decay_pct: 0.05,
      start_decay_after_sec: 60,
      compound_step_sec: 60,
      start_time: "2022-01-12T00:00:00",
      mint_price_min: "1.0000 EOS",
      mint_price_increase_by_rank: "0.1000 EOS",
      max_mint_per_round: 7,
      bronze_to_silver: 20,
      silver_to_gold: 10
    }
    await doAction('setconfig', { cfg })
  },
  async clrconfig() {
    await doAction('clrconfig')
  },
  async clrstate() {
    await doAction('clrstate')
  },
  async clrround(round_id) {
    await doAction('clrround', { round_id })
  },
  async clrrounds() {
    await doAction('clrrounds')
  },
  async rewardround(round_id) {
    await doAction('rewardround', { round_id })
  },
  async clraccounts() {
    await doAction('clraccounts')
  }
}


if (require.main == module) {
  if (Object.keys(methods).find(el => el === process.argv[2])) {
    console.log("Starting:", process.argv[2])
    methods[process.argv[2]](...process.argv.slice(3)).catch((error) => console.error(error))
      .then((result) => console.log('Finished'))
  } else {
    console.log("Available Commands:")
    console.log(JSON.stringify(Object.keys(methods), null, 2))
  }
}
module.exports = methods
