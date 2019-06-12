'use strict'

const BN = require('bn.js')
const u = require('../../../../util')

async function handleTransfer(
  platformState,
  secretStore,
  k0,
  emitter,
  txHash,
  in0sn,
  in1sn,
  out0cm,
  out1cm,
  out0data,
  out1data,
  nextRoot
) {
  const outputs = [ [ out0cm, out0data ], [ out1cm, out1data ] ]
  for (let i = 0; i < 2; i = i + 1) {
    const decryptionResult = await k0.decryptNote(secretStore, outputs[i][1])
    if (decryptionResult.success) {
      secretStore.addNoteInfo(
        outputs[i][0],
        secretStore.getAPk(),
        decryptionResult.rho,
        decryptionResult.r,
        decryptionResult.v
      )
    }
  }

  await platformState.add(
    u.buf2hex(txHash),
    [ in0sn, in1sn ],
    [ out0cm, out1cm ],
    nextRoot
  )
  emitter.emit('transferProcessed', txHash)
}

module.exports = handleTransfer
