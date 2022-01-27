const fs = require('fs-extra')
const conf = require('../eosioConfig')
const env = require('../.env.js')
const { api, tapos, doAction, getFullTable } = require('./lib/eosjs')()
const activeChain = process.env.CHAIN || env.defaultChain
const contractAccount = conf.accountName[activeChain]
const mintData = {
  genesis: {
    bronze: 3642,
    silver: 3643,
    gold: 3644
  },
  elements: {
    bronze: 3648,
    silver: 3649,
    gold: 3650
  }
}

function chunkArray(arr, chunkSize) {
  const res = [];
  while (arr.length > 0) {
    const chunk = arr.splice(0, chunkSize);
    res.push(chunk);
  }
  return res;
}

function mintAction(template_id, new_asset_owner, schema_name) {
  {
    return {
      authorized_minter: contractAccount,
      collection_name: contractAccount,
      schema_name,
      template_id,
      new_asset_owner,
      immutable_data: [],
      mutable_data: [],
      tokens_to_back: []
    }
  }
}

async function mintList(listFile, type) {
  try {
    const list = await fs.readJson(listFile)
    const authorization = [{ actor: contractAccount, permission: 'active' }]
    const actions = []
    for (const data of Object.entries(list)) {
      const account = data[0]
      const info = data[1]
      if (info.bronze) {
        for (let i = 0; i < info.bronze; i++) {
          actions.push({ account: 'atomicassets', name: 'mintasset', data: mintAction(mintData[type].bronze, account, type), authorization })
        }
      }
      if (info.silver) {
        for (let i = 0; i < info.silver; i++) {
          actions.push({ account: 'atomicassets', name: 'mintasset', data: mintAction(mintData[type].silver, account, type), authorization })
        }
      }
      if (info.gold) {
        for (let i = 0; i < info.gold; i++) {
          actions.push({ account: 'atomicassets', name: 'mintasset', data: mintAction(mintData[type].gold, account, type), authorization })
        }
      }

    }
    // await fs.writeJson('./actions.json', actions, { spaces: 2 })
    const actionParts = chunkArray(actions, 100)
    console.log(actionParts.length);
    let i = 0
    for (let actions of actionParts) {
      console.log(i);
      if (i == 0) {
        i++
        continue
      }
      const result = await api.transact({ actions }, tapos)
      console.log(result.transaction_id);
      i++
    }
  } catch (error) {
    console.error(error)
  }
}
// mintList('./elementalList.json', 'elements')
// mintList('./genesisList.json', 'genesis')
