# K0

K0 is an implementation of [Zerocash](http://zerocash-project.org/) for smart contract platforms. Currently Ethereum and Hyperledger Fabric are supported. The protocol has been modified slightly to enable an additional use case, namely the use of a predefined private payment in a secondary smart contract. For more information please read our [technical description](https://appliedblockchain.com/k0.pdf).

__WARNING! This is alpha-stage software. It has not been security-audited yet.__

A quickstart guide to run some integration tests is provided [below](#quickstart). If you want to start developing with/on K0 (not needed for the quickstart), please consult the README files in the following subdirectories:
- [cpp: Build and run C++ components](cpp)
- [js/packages/k0-integration-tests-eth: Run Ethereum integration tests](js/packages/k0-integration-tests-eth)
- [js/packages/k0-integration-tests-fabric: Run Fabric integration tests](js/packages/k0-integration-tests-fabric)

## Quickstart

The easiest way to see the code in action is to run the Ethereum integration tests using the provided Docker Compose file. However, first we need to run the "trusted setup", also using Docker (this might take a couple of minutes):

```
rm -rf /tmp/k0keys
mkdir /tmp/k0keys
for circuit in commitment transfer addition withdrawal example
do
  docker run -it -v /tmp/k0keys/:/tmp/k0keys/ appliedblockchain/k0-setup:v0.0.32 $circuit 4 /tmp/k0keys/${circuit}\_pk /tmp/k0keys/${circuit}_vk
  docker run -it -v /tmp/k0keys/:/tmp/k0keys/ appliedblockchain/k0-convert-vk:v0.0.32 /tmp/k0keys/${circuit}\_vk /tmp/k0keys/${circuit}\_vk_alt
done
```

Then, spin up the Docker containers:

```
docker-compose up -d
```

Monitor the tests:
```
docker-compose logs -f runner
```

Optional: Monitor the whole Docker Compose network (maybe in a separate terminal window):
```
docker-compose logs -f --tail 100
```

After the tests have run, shut down the network:
```
docker-compose down
```
