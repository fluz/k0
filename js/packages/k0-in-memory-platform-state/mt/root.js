const request = require('./request')
const u = require('@appliedblockchain/k0-util')

async function root(jc) {
  const res = await request(jc, 'root')
  return u.hex2buf(res)
}

module.exports = root