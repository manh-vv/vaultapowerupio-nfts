const conf = require('./eosioConfig')
const env = require('../.env.js')
const { api, tapos } = require('./lib/eosjs')(env.keys[env.defaultChain], conf.endpoints[env.defaultChain][0])
const contractAccount = conf.accountName[env.defaultChain]
var watchAccountSample = require('./lib/sample_watchaccount')
function chainName() {
  if (env.defaultChain == 'jungle') return 'jungle3'
  if (env.defaultChain == 'telosTest') return 'telos-test'
  else return env.defaultChain
}

async function doAction(name, data, account, auth) {
  try {
    if (!data) data = {}
    if (!account) account = contractAccount
    if (!auth) auth = account
    console.log("Do Action:", name, data)
    const authorization = [{ actor: auth, permission: 'active' }]
    const result = await api.transact({
      // "delay_sec": 0,
      actions: [{ account, name, data, authorization }]
    }, tapos)
    const txid = result.transaction_id
    console.log(result)
    console.log(`https://${chainName()}.bloks.io/transaction/` + txid)
    // console.log(`https://jungle.eosq.eosnation.io/tx/${txid}`);
    // console.log(txid)r
    return result
  } catch (error) {
    console.error(error.toString())
    if (error.json) console.error("Logs:", error.json?.error?.details[1]?.message)
  }
}

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
      start_time: "2022-01-13T18:00:00",
      max_rounds: 5
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
