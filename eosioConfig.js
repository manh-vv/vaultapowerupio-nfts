module.exports = {
  chains: ['eos', 'kylin', 'wax', 'jungle', 'telosTest'],
  endpoints: {
    eos: ['https://eos.greymass.com', 'https://eos.eosn.io', 'https://eos.dfuse.eosnation.io/'],
    kylin: ['https://kylin.eosn.io', 'https://kylin.eossweden.org'],
    jungle: ['https://jungle3.cryptolions.io', 'https://jungle3.greymass.com'],
    telosTest: ['https://testnet.telos.caleos.io']
  },
  accountName: {
    telosTest: "eospwrupnfts",
    jungle: "eospwrupnfts",
    eos: "powerup.nfts"
  },
  contractName: 'donations',
  cppName: 'donations'
}
