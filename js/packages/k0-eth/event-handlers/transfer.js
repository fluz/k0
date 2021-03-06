'use strict'

const BN = require('bn.js')
const u = require('@appliedblockchain/k0-util')

async function handleTransfer(server, eventEmitter, event) {
  const [sn_in_0, sn_in_1, cm_out_0, cm_out_1, new_root] = await Promise.all(
    ['sn_in_0', 'sn_in_1', 'cm_out_0', 'cm_out_1', 'new_root'].map(label => {
      return server.unpack256Bits(
        new BN(event.returnValues[label][0].toString()),
        new BN(event.returnValues[label][1].toString())
      )
    })
  )
  const [data_out_0, data_out_1] = await Promise.all(
    ['data_out_0', 'data_out_1'].map(label => {
      return u.hex2buf(event.returnValues[label])
    })
  )
  eventEmitter.emit(
    'transfer',
    u.hex2buf(event.transactionHash),
    sn_in_0,
    sn_in_1,
    cm_out_0,
    cm_out_1,
    data_out_0,
    data_out_1,
    new_root
  )
}

module.exports = handleTransfer
