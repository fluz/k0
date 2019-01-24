const assert = require('assert')
const crypto = require('crypto')
const execAsync = require('../exec-async')
const path = require('path')
const proofFromFile = require('../proof-from-file')
const sendTransaction = require('../send-transaction')
const Web3 = require('web3')

const baseDir = path.join(__dirname, '..', '..')
const cppUtilDir = path.join(baseDir, 'cpp', 'build', 'src')

async function convertVk(vkPath, vkAltPath) {
  const executablePath = path.join(cppUtilDir, 'convert_vk')
  const command = `${executablePath} ${vkPath} ${vkAltPath}`
  return execAsync(command)
}

async function convertProof(proofPath, proofAltPath) {
  const executablePath = path.join(cppUtilDir, 'convert_proof')
  const command = `${executablePath} ${proofPath} ${proofAltPath}`
  return execAsync(command)
}

function paths(label) {
  const tmpDir = process.env.TMP_DIR || path.join('/', 'tmp')
  const baseDir = path.join(__dirname, '..', '..')
  return {
    tmpDir,
    baseDir,
    cppDir: path.join(baseDir, 'cpp', 'build', 'src', 'examples', label),
    cppUtilDir: path.join(baseDir, 'cpp', 'build', 'src'),
    pk: path.join(tmpDir, `${label}_pk`),
    vk: path.join(tmpDir, `${label}_vk`),
    vkAlt: path.join(tmpDir, `${label}_vk_alt`),
    proof: path.join(tmpDir, `${label}_proof`),
    proofAlt: path.join(tmpDir, `${label}_proof_alt`)
  }
}

async function pack256Bits(hex) {
  const executablePath = path.join(cppUtilDir, 'pack_256_bits')
  const command = `${executablePath} ${hex}`
  const result = await execAsync(command)
  return result.stdout.trim().split(',')
}

async function verifyOnChainWithProof(web3, contract, proof, args, expectSuccess) {
  const data = contract.methods.verifyProof(...proof, args).encodeABI()
  const receipt = await sendTransaction(web3, contract._address, data)
  if (expectSuccess) {
    assert(receipt.status)
    assert(receipt.logs.length === 1)
    // event "Verified(string)"
    assert(receipt.logs[0].topics[0] === '0x3f3cfdb26fb5f9f1786ab4f1a1f9cd4c0b5e726cbdfc26e495261731aad44e39')
    console.log(`Proof verified. Cost of verification: ${receipt.gasUsed} gas.`)
  } else {
    assert(receipt.status)
    assert(receipt.logs.length === 0)
  }
}

async function verifyOnChain(web3, contract, proofAltPath, args, expectSuccess) {
  const proof = await proofFromFile(proofAltPath)
  await verifyOnChainWithProof(web3, contract, proof, args, expectSuccess)
}

function randomBytes(len) {
  return '0x' + crypto.randomBytes(len).toString('hex')
}

async function sha256Instance() {
  const pyScriptPath = path.join(baseDir, 'python', 'sha256_instance.py')
  const pyResult = await execAsync(`python ${pyScriptPath}`)
  return pyResult.stdout.split(',')
}

function initWeb3() {
  const endpointFromEnv = process.env.ETHEREUM_JSONRPC_ENDPOINT  
  return new Web3( endpointFromEnv || 'http://localhost:8545/')
}

module.exports = {
  convertVk,
  convertProof,
  initWeb3,
  randomBytes,
  paths,
  pack256Bits,
  proofFromFile,
  sha256Instance,
  verifyOnChain,
  verifyOnChainWithProof
}
