module.exports = {
  chains: ['eos', 'kylin', 'wax', 'jungle'],
  endpoints: {
    eos: ['https://eos.greymass.com', 'https://eos.eosn.io', 'https://eos.dfuse.eosnation.io/'],
    kylin: ['https://kylin.eosn.io', 'https://kylin.eossweden.org'],
    jungle: ['https://jungle3.greymass.com'],
    telos: ['https://telos.caleos.io'],
    telosTest: ['https://testnet.telos.caleos.io'],
  },
  accountName: {
    eos: 'eospowerupio',
    kylin: 'eospowerupio',
    jungle: 'eospwrupnfts',
    telosTest: 'eospwrupnfts'
  },
  contractName: 'donations',
  cppName: 'donations'
}
