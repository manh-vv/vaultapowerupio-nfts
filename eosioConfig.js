module.exports = {
  chains: ['eos', 'jungle', 'kylin', 'telosTest'],
  endpoints: {
    eos: ['https://eos.api.animus.is', 'https://eos.greymass.com', 'https://eos.eosn.io', 'https://eos.dfuse.eosnation.io/'],
    kylin: ['https://kylin.eosn.io', 'https://kylin.eossweden.org'],
    jungle: ['https://jungle4.genereos.io', 'https://jungle4.cryptolions.io'],
    telosTest: ['https://testnet.telos.caleos.io']
  },
  explorers: {
    eos: 'https://coffe.bloks.io',
    kylin: 'https://kylin.bloks.io',
    jungle: 'https://local.bloks.io/?nodeUrl=http%3A%2F%2Fjungle4.cryptolions.io&systemDomain=eosio&hyperionUrl=https%3A%2F%2Fjungle4history.cryptolions.io'
  },
  accountName: {
    telosTest: "vaultapwnfts",
    jungle: "vaultapwnfts",
    eos: "vaultapwnfts"
  },
  contractName: 'donations',
  cppName: 'donations'
}
