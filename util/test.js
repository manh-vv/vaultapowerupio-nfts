const conf = require('../eosioConfig')
const env = require('../.env.js')
const { api, tapos, doAction, getFullTable } = require('./lib/eosjs')(env.keys[env.defaultChain], conf.endpoints[env.defaultChain][0])
const contractAccount = conf.accountName[env.defaultChain]
const contractActions = require('./do.js')

const methods = {
  async getRound() {
    const configTbl = await api.rpc.get_table_rows({ code: contractAccount, scope: contractAccount, table: "config" }).catch(err => console.error(err))
    const conf = configTbl.rows[0]
    const startTime = Date.parse(conf.start_time.split(".")[0] + ".000+00:00") / 1000
    console.log('startTime', startTime);
    const now = (Date.now() / 1000)
    console.log('now', now);
    const elapsed = now - startTime
    console.log('elapsed:', elapsed);
    const round = Math.floor(elapsed / conf.round_length_sec) + 1
    console.log("current Round: ", round);
  },
  async claimAll() {
    const results = await getFullTable({ code: contractAccount, scope: contractAccount, table: 'claimed' })
    const filtered = results.filter(el => el.bronze_unclaimed > 0)
    for (const row of filtered) {
      await contractActions.claim(row.account)
    }
  },
  async rewardAllRounds() {
    const result = await getFullTable({ code: contractAccount, scope: contractAccount, table: "rounds" })
    console.log(result.length)
    let i = 0
    for (const round of result) {
      if (round.rewarded == 1) continue
      await contractActions.rewardround(round.id)
    }
    await this.claimAll()
  },

  async transfer(quantity, from, to = contractAccount, memo = "", contract = 'eosio.token') {
    const data = {
      from,
      to,
      quantity,
      memo
    }
    await doAction('transfer', data, contract, from)
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
