'use strict'

const _ = require('lodash')
const BN = require('bn.js')
const crypto = require('crypto')
const log4js = require('log4js')
const { expect } = require('code')
const waitPort = require('wait-port')
const jayson = require('jayson/promise')

const u = require('../../util')
const makeK0 = require('../../k0')
const testUtil = require('../util')
const makeEthPlatform = require('../../eth')
const serverReady = require('../../client/ready')
const makeSecretStore = require('../../secret-store')
const sendTransaction = require('../../send-transaction')
const makePlatformState = require('../../platform-state')
const compileContracts = require('../helpers/compile-contracts')
const initEventHandlers = require('./helpers/event-handlers')
const signTransaction = require('../../eth/sign-transaction')

const assert = require('assert')

const k0Ports = [ 4000, 5000, 6000 ]
const platformPorts = [ 4100, 5100, 6100 ]

let web3

describe('Ethereum integration test replicating the K0 demo', function () {
  this.timeout(3600 * 1000)

  let alice, bob, carol // web3 accounts, and secretKeys
  let artefacts
  let addresses // contract and user addresses, indexed by name
  let verifierAddresses
  let dollarCoin, carToken, mvppt
  let tokenMaster, carManufacturer

  const carId = 1
  const numInitialNotes = 2

  // close web3 websocket so the test can exit
  after(() => {
    if (web3 && web3.currentProvider) {
      web3.currentProvider.connection.close()
    }
  })

  // Make the accounts, deploy the contracts
  before(async () => {
    const logger = log4js.getLogger()
    logger.level = process.env.LOG_LEVEL || 'info'

    process.on('unhandledRejection', error => {
      logger.error(error)
      process.exit(1)
    })

    let ready = false
    console.log('TEST: waiting for all the servers to respond')
    while (!ready) {
      try {
        await Promise.all([
          waitPort({ port: k0Ports[0] }),
          waitPort({ port: k0Ports[1] }),
          waitPort({ port: k0Ports[2] }),
          waitPort({ port: 8546 }), // parity
          waitPort({ port: platformPorts[0] }),
          waitPort({ port: platformPorts[1] }),
          waitPort({ port: platformPorts[2] })
        ])

        await Promise.all([
          serverReady(jayson.client.http({ port: k0Ports[0] })),
          serverReady(jayson.client.http({ port: k0Ports[1] })),
          serverReady(jayson.client.http({ port: k0Ports[2] }))
        ])

        ready = true
      } catch (err) {
        process.stdout.write('.')
        await u.wait(10000)
      }
    }

    web3 = testUtil.initWeb3()
    // DollarCoin minter
    tokenMaster = web3.eth.accounts.create()

    // CarToken minter
    carManufacturer = web3.eth.accounts.create()

    // contract artefacts
    artefacts = await compileContracts()

    // ERC-20 payment token
    dollarCoin = await testUtil.deployContract(
      web3,
      artefacts.DollarCoin,
      [],
      tokenMaster
    )

    // ERC-721 token representing cars
    carToken = await testUtil.deployContract(
      web3,
      artefacts.CarToken,
      [],
      carManufacturer
    )

    verifierAddresses = await Promise.all(
      [
        'CommitmentVerifier',
        'AdditionVerifier',
        'TransferVerifier',
        'WithdrawalVerifier'
      ].map(async name => {
        const contract = await testUtil.deployContract(web3, artefacts[name])
        return contract._address
      })
    )
    ;[ alice, bob, carol ] = _.times(3, () => {
      const account = web3.eth.accounts.create()
      return { account }
    })

    alice.platformState = await makePlatformState(platformPorts[0])
    bob.platformState = await makePlatformState(platformPorts[1])
    carol.platformState = await makePlatformState(platformPorts[2])

    const initialRoot = await alice.platformState.merkleTreeRoot()

    // Deploying coin contract
    mvppt = await testUtil.deployContract(web3, artefacts.MVPPT, [
      dollarCoin._address,
      ...verifierAddresses,
      await testUtil.pack256Bits(u.buf2hex(initialRoot))
    ])

    const moneyShower = await testUtil.deployContract(
      web3,
      artefacts.MoneyShower
    )

    // Make some money
    await sendTransaction(
      web3,
      dollarCoin._address,
      dollarCoin.methods
        .mint(
          tokenMaster.address,
          '1000000000000000000000000000000000000000'
        )
        .encodeABI(),
      50000000,
      tokenMaster
    )

    await sendTransaction(
      web3,
      dollarCoin._address,
      dollarCoin.methods
        .approve(
          moneyShower._address,
          web3.utils.toWei((numInitialNotes * 3000000).toString())
        )
        .encodeABI(),
      50000000,
      tokenMaster
    )


    await sendTransaction(
      web3,
      moneyShower._address,
      moneyShower.methods.transfer(
        dollarCoin._address,
        _.map([ alice, bob ], x => x.account.address),
        _.times(2, () => '1000000000000')
      ).encodeABI(),
      5000000,
      tokenMaster
    )

    // Make a car for carol
    await sendTransaction(
      web3,
      carToken._address,
      carToken.methods.mint(carol.account.address, carId).encodeABI(),
      5000000,
      carManufacturer
    )

    addresses = {
      DollarCoin: dollarCoin._address,
      CarToken: carToken._address,
      MVPPT: mvppt._address,
      alice: alice.account.address,
      bob: alice.account.address,
      carol: alice.account.address
    }

    alice.k0Eth = await makeEthPlatform(web3, u.hex2buf(addresses.MVPPT))
    bob.k0Eth = await makeEthPlatform(web3, u.hex2buf(addresses.MVPPT))
    carol.k0Eth = await makeEthPlatform(web3, u.hex2buf(addresses.MVPPT))


    alice.secretKey = crypto.randomBytes(32)
    bob.secretKey = crypto.randomBytes(32)
    carol.secretKey = crypto.randomBytes(32)

    alice.k0 = await makeK0(k0Ports[0])
    bob.k0 = await makeK0(k0Ports[1])
    carol.k0 = await makeK0(k0Ports[2])

    alice.secretStore = await makeSecretStore(k0Ports[0], alice.secretKey)
    bob.secretStore = await makeSecretStore(k0Ports[1], bob.secretKey)
    carol.secretStore = await makeSecretStore(k0Ports[2], carol.secretKey)

    alice.emitter = initEventHandlers(
      alice.platformState,
      alice.secretStore,
      alice.k0Eth,
      alice.k0
    )

    bob.emitter = initEventHandlers(
      bob.platformState,
      bob.secretStore,
      bob.k0Eth,
      bob.k0
    )

    carol.emitter = initEventHandlers(
      carol.platformState,
      carol.secretStore,
      carol.k0Eth,
      carol.k0
    )

    console.log('Test Suite Initialized.')
  })

  async function checkRootsConsistency() {
    const ethRoot = await alice.k0Eth.merkleTreeRoot()

    const [ root1, root2, root3 ] = await Promise.all([
      alice.platformState.merkleTreeRoot(),
      bob.platformState.merkleTreeRoot(),
      carol.platformState.merkleTreeRoot()
    ])

    assert(ethRoot.equals(root1))
    assert(ethRoot.equals(root2))
    assert(ethRoot.equals(root3))
  }

  // Consume  $coin in exchange for CMs
  async function approveAndDeposit(
    user,
    values
  ) {
    // aprove an amounts to mvptt
    const total = values.reduce((acc, el) => acc.add(el), new BN('0'))

    const approveTx = await signTransaction(
      web3,
      u.hex2buf(dollarCoin._address),
      u.hex2buf(
        dollarCoin.methods
          .approve(addresses.MVPPT, total.toString())
          .encodeABI()
      ),
      5000000,
      u.hex2buf(user.account.privateKey)
    )
    await web3.eth.sendSignedTransaction(u.buf2hex(approveTx))

    for (let i = 0; i < values.length; i++) {
      const v = values[i]

      const data = await user.k0.prepareDeposit(
        user.platformState,
        user.secretStore.getAddress(),
        v
      )

      const waitForDeposit = testUtil.awaitEvent(user.emitter, 'depositProcessed')
      const depositTx = await user.k0Eth.deposit(
        u.hex2buf(user.account.privateKey),
        v,
        data.k,
        data.cm,
        data.ciphertext,
        data.nextRoot,
        data.commitmentProofAffine,
        data.additionProofAffine
      )

      await web3.eth.sendSignedTransaction(u.buf2hex(depositTx))

      await waitForDeposit
    }
  }

  let values
  it('Can mint the CMS', async () => {
    // DEPOSIT TEST
    values = _.times(
      numInitialNotes,
      () => new BN(_.random(50).toString() + '000')
    )

    await approveAndDeposit(
      alice,
      values
    )
    await checkRootsConsistency()

    // check that we have now "numInitialNotes" cm in the merkle tree
    expect(await alice.platformState.currentState().cmList.length).to.equal(
      numInitialNotes
    )

    await u.wait(1000)

    await approveAndDeposit(
      bob,
      values
    )
    await checkRootsConsistency()
  })

  async function getInputNote(platformState, secretStore, index) {
    const cm = platformState.cmAtIndex(new BN(index))
    const info = secretStore.getNoteInfo(cm)

    return {
      address: new BN(index),
      cm,
      ...info
    }
  }

  it('allows alice to transfer funds to bob', async () => {
    await checkRootsConsistency()
    const in0 = await getInputNote(alice.platformState, alice.secretStore, 0)
    const in1 = await getInputNote(alice.platformState, alice.secretStore, 1)

    const totalValue = in0.v.add(in1.v)

    const out0 = {
      address: bob.secretStore.getAddress(),
      v: totalValue,
      rho: crypto.randomBytes(32),
      r: crypto.randomBytes(48)
    }

    const out1 = {
      address: alice.secretStore.getAddress(),
      v: new BN('0'),
      rho: crypto.randomBytes(32),
      r: crypto.randomBytes(48)
    }

    // zero address since we are not trading
    const callee = u.hex2buf(u.ZERO_ADDRESS)

    const transferData = await alice.k0.prepareTransfer(
      alice.platformState,
      alice.secretStore,
      new BN(0), // cms index
      new BN(1),
      out0,
      out1,
      u.hex2buf(u.ZERO_ADDRESS),
      callee
    )

    alice.secretStore.addSNToNote(in0.cm, transferData.input_0_sn)
    alice.secretStore.addSNToNote(in1.cm, transferData.input_1_sn)

    const rootBefore = await alice.platformState.merkleTreeRoot()
    const labelBefore = bob.platformState.currentStateLabel()
    const tmpLabel =
      'temporary_mt_addition_' + crypto.randomBytes(16).toString('hex')
    await alice.platformState.add(
      tmpLabel,
      [],
      [ transferData.output_0_cm, transferData.output_1_cm ]
    )
    const newRoot = await alice.platformState.merkleTreeRoot()
    await alice.platformState.rollbackTo(labelBefore)

    const finalRoot = await alice.platformState.merkleTreeRoot()
    assert(finalRoot.equals(rootBefore))

    const ethParams = [
      crypto.randomBytes(32),
      transferData.input_0_sn,
      transferData.input_1_sn,
      transferData.output_0_cm,
      transferData.output_1_cm,
      transferData.output_0_ciphertext,
      transferData.output_1_ciphertext,
      newRoot,
      callee,
      transferData.proofAffine
    ]

    const tx = await alice.k0Eth.transfer(...ethParams)

    const avNotesBefore = [alice, bob, carol].map(who => {
      return [
        who.secretStore.getAPk(),
        who.secretStore.getAvailableNotes().length
      ]
    })

    await u.wait(5000)
    const transferProcessed = testUtil.awaitEvent(bob.emitter, 'transferProcessed')
    const receipt = await web3.eth.sendSignedTransaction(u.buf2hex(tx))
    expect(receipt.status).to.equal(true)

    await transferProcessed
    await u.wait(5000)

    const avNotesAfter = [alice, bob, carol].map(who => {
      return [
        who.secretStore.getAPk(),
        who.secretStore.getAvailableNotes().length
      ]
    })
    expect(
      bob.secretStore.getAvailableNotes().length
    ).to.equal(numInitialNotes + 1)
  })

  it('allows carol to sell her car to bob', async () => {
    // Deploying The trading contract
    const bobNotes = bob.secretStore.getAvailableNotes()
    const [ in0, in1 ] = bobNotes
    const in0V = bobNotes[0].v
    const in1V = bobNotes[1].v

    // average between the bigest one and the sum
    const carPrice = in0V.gt(in1V) ?
      in0V.add(in0V.add(in1V)).div(new BN(2)) :
      in1V.add(in1V.add(in0V)).div(new BN(2))

    const change = in0V.add(in1V).sub(carPrice)

    const { rho, r, cm } = await carol.k0.generatePaymentData(
      carol.secretStore,
      carPrice
    )

    const cmPacked = await u.pack256Bits(cm)

    const tradeContract = await testUtil.deployContract(
      web3,
      artefacts.HiddenPriceCarTrade,
      [
        carToken._address,
        mvppt._address,
        (new BN(carId)).toString(),
        u.buf2hex(cmPacked[0]),
        u.buf2hex(cmPacked[1])
      ],
      carol.account
    )

    // Aprove the car to the trading contract
    const approvalTxData = carToken.methods.approve(
      tradeContract._address, carId.toString()
    ).encodeABI()

    const txData = await carol.account.signTransaction({
      to: carToken._address,
      data: approvalTxData,
      gas: 1000000
    })

    await web3.eth.sendSignedTransaction(txData.rawTransaction)

    const out0 = {
      address: carol.secretStore.getAddress(),
      v: carPrice,
      rho, // Generated payment data in step1
      r // Generated payment data in step1
    }

    const out1 = { // Change back to bob
      address: bob.secretStore.getAddress(),
      v: change,
      rho: crypto.randomBytes(32),
      r: crypto.randomBytes(48)
    }

    const in0Id = bob.platformState.indexOfCM(in0.cm)
    const in1Id = bob.platformState.indexOfCM(in1.cm)

    // Prepare the transfer
    const transferData = await bob.k0.prepareTransfer(
      bob.platformState,
      bob.secretStore,
      in0Id, // cms index in the Merkle Tree
      in1Id,
      out0,
      out1,
      u.hex2buf(tradeContract._address)
    )

    // execute the transfer
    bob.secretStore.addSNToNote(in0.cm, transferData.input_0_sn)
    bob.secretStore.addSNToNote(in1.cm, transferData.input_1_sn)

    const rootBefore = await bob.platformState.merkleTreeRoot()
    const labelBefore = await bob.platformState.currentStateLabel()
    const tmpLabel =
      'temporary_mt_addition_' + crypto.randomBytes(16).toString('hex')
    await bob.platformState.add(
      tmpLabel,
      [],
      [ transferData.output_0_cm, transferData.output_1_cm ]
    )
    const newRoot = await bob.platformState.merkleTreeRoot()
    await bob.platformState.rollbackTo(labelBefore)

    const finalRoot = await bob.platformState.merkleTreeRoot()
    assert(finalRoot.equals(rootBefore))

    const ethParams = [
      u.hex2buf(bob.account.privateKey),
      transferData.input_0_sn,
      transferData.input_1_sn,
      transferData.output_0_cm,
      transferData.output_1_cm,
      transferData.output_0_ciphertext,
      transferData.output_1_ciphertext,
      newRoot,
      u.hex2buf(tradeContract._address),
      transferData.proofAffine
    ]

    let carOwnwer = await carToken.methods.ownerOf(carId).call()

    const tx = await bob.k0Eth.transfer(...ethParams)

    const transferForDeposit = testUtil.awaitEvent(bob.emitter, 'transferProcessed')
    const receipt = await web3.eth.sendSignedTransaction(u.buf2hex(tx))

    expect(receipt.status).to.equal(true)

    await transferForDeposit
    await u.wait(1000)

    // Test that the ownership got transfered
    carOwnwer = await carToken.methods.ownerOf(carId).call()

    expect(carOwnwer).to.equal(bob.account.address)
  })
})
