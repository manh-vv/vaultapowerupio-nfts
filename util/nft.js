
const conf = require('../eosioConfig')
const env = require('../.env.js')
const { api, tapos, doAction } = require('./lib/eosjs')()
const activeChain = process.env.CHAIN || env.defaultChain
const defaultParams = { scope: conf.accountName[activeChain], code: conf.accountName[activeChain] }
const meta = require('./nftMetadata')
const defaultCollectionData = [
  { key: "name", value: ["string", "EOS PowerUp NFTs"] },
  { key: "img", value: ["string", "QmbncqcDy6pdNH6J7qTtdkmYrhyQ9BfaUwE4bbUPXURpE8"] },
  { key: "description", value: ["string", "NFTs created by eospowerup.io."] },
  { key: "url", value: ["string", "https://eospowerup.io"] }
]

const methods = {
  async transfer(from, to, asset, memo) {
    await doAction('transfer', {
      from,
      to,
      asset_ids: [parseInt(asset)],
      memo
    }, 'atomicassets', from)
  },
  async createCollection() {
    await doAction('createcol', {
      author: defaultParams.code,
      collection_name: 'eospwrupnfts',
      allow_notify: true,
      authorized_accounts: [defaultParams.code],
      notify_accounts: [defaultParams.code],
      market_fee: 0.1,
      data: defaultCollectionData
    }, 'atomicassets', defaultParams.code)
  },
  async setCollectionData() {
    await doAction('setcoldata', {
      collection_name: "eospwrupnfts",
      data: [
        { key: "name", value: ["string", "EOS PowerUp NFTs"] },
        { key: "img", value: ["string", "QmbncqcDy6pdNH6J7qTtdkmYrhyQ9BfaUwE4bbUPXURpE8"] },
        { key: "description", value: ["string", "NFTs created by eospowerup.io."] },
        { key: "url", value: ["string", "https://eospowerup.io"] }
      ]
    }, 'atomicassets', defaultParams.code)
  },
  async createSchema() {
    await doAction('createschema', {
      authorized_creator: defaultParams.code,
      collection_name: 'eospwrupnfts',
      schema_name: 'genesis',
      schema_format: [
        { name: 'name', type: 'string' },
        { name: 'img', type: 'image' },
        { name: 'rarity', type: 'string' },
        { name: 'details', type: 'string' },
        { name: 'url', type: 'string' }
      ]
    }, 'atomicassets', defaultParams.code)
  },
  async createTemplate() {
    await doAction('createtempl', {
      authorized_creator: defaultParams.code,
      collection_name: 'eospwrupnfts',
      schema_name: "genesis",
      transferable: true,
      burnable: true,
      max_supply: 0,
      immutable_data: meta.genesis.gold
    }, 'atomicassets', defaultParams.code)
  },
  async mint() {
    await doAction('mintasset', {
      authorized_minter: defaultParams.code,
      collection_name: 'eospwrupnfts',
      schema_name: 'genesis',
      template_id: '324887',
      new_asset_owner: 'eospwrupnfts',
      immutable_data: [],
      mutable_data: [],
      tokens_to_back: []
    }, 'atomicassets', defaultParams.code)
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
