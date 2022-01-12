const conf = require('../eosioConfig')
const env = require('../.env.js')
const { api, tapos, doAction } = require('./lib/eosjs')()
const activeChain = process.env.CHAIN || env.defaultChain
const contractAccount = conf.accountName[activeChain]

const methods = {
  async updateAuth() {
    const auth = {
      "threshold": 1,
      "keys": [{ key: "EOS6FoTSwiKk27SJ1kANdJFmso3KbECASAMDpEka4dG9p1ub6GqiH", weight: 1 }],
      "accounts": [{ "permission": { "actor": contractAccount, "permission": "eosio.code" }, "weight": 1 }],
      "waits": []
    }
    await doAction('updateauth', { account: contractAccount, auth, parent: 'owner', permission: 'active' }, 'eosio', contractAccount)
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
