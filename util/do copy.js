const conf = require('./eosioConfig')
const env = require('../.env.js')
const { api, tapos, doAction } = require('./lib/eosjs')(env.keys[env.defaultChain], conf.endpoints[env.defaultChain][0])
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
    scope = contractAccount;
    await doAction('clrleaderb')
  },
  async setconfig(cfg) {
    cfg = {
      round_length_sec: 60 * 5,
      minimum_donation: "1.0000 EOS",
      enabled: 1,
      compound_decay_pct: 0.05,
      start_decay_after_sec: 60,
      compound_step_sec: 60,
      start_time: "2022-01-10T18:00:00",
      max_rounds: 5
    }
    await doAction('setconfig')
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
