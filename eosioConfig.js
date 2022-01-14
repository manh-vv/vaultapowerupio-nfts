module.exports = {
  chains: ['eos', 'kylin', 'wax', 'jungle'],
  endpoints: {
    eos: ['https://eos.greymass.com', 'https://eos.eosn.io', 'https://eos.dfuse.eosnation.io/'],
    telosTest: ['https://testnet.telos.caleos.io'],
    waxTest: ['https://testnet.wax.eosdetroit.io', 'https://testnet.waxsweden.org']
  },
  accountName: {
    eos: 'eospwrupnfts',
    waxTest: 'eospwrupnfts',
    telosTest: 'eospwrupnfts'
  },
  contractName: 'donations',
  cppName: 'donations'
}
