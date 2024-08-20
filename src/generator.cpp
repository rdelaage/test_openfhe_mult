#include <openfhe.h>
#include <ciphertext-ser.h>
#include <cryptocontext-ser.h>
#include <key/key-ser.h>
#include <scheme/bfvrns/bfvrns-ser.h>

#include "config.hpp"

using namespace lbcrypto;

int main() {
        // Parse the configuration
        auto config = Config("config.json");
        std::cout << config;

        // Generate and serialize cryptocontext
        std::cout << "Generate cryptocontext" << std::endl;

        CCParams<CryptoContextBFVRNS> parameters;

        parameters.SetPlaintextModulus(65537);
        parameters.SetSecurityLevel(lbcrypto::SecurityLevel::HEStd_128_classic);
        parameters.SetStandardDeviation(3.2);
        parameters.SetSecretKeyDist(UNIFORM_TERNARY);
        parameters.SetMultiplicativeDepth(4);
        parameters.SetBatchSize(16);
        parameters.SetDigitSize(30);
        parameters.SetScalingModSize(60);
        parameters.SetMultiplicationTechnique(HPSPOVERQLEVELED);

        CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);
        cryptoContext->Enable(PKE);
        cryptoContext->Enable(KEYSWITCH);
        cryptoContext->Enable(LEVELEDSHE);
        cryptoContext->Enable(ADVANCEDSHE);
        cryptoContext->Enable(MULTIPARTY);

        std::cout << "Generated cryptocontext" << std::endl;
        
        // Perform Key Generation Operation

        std::cout << "Perform Key Generation Operation" << std::endl;
        
        std::vector<KeyPair<DCRTPoly>> keyPairs;
        // generate the kp for all parties
        for ( auto& party: config.parties ) {
                KeyPair<DCRTPoly> kp;
                if ( keyPairs.empty() ) {
                        kp = cryptoContext->KeyGen();
                } else {
                        kp = cryptoContext->MultipartyKeyGen(keyPairs.back().publicKey);
                }

                if (!Serial::SerializeToFile(party.sk, kp.secretKey, SerType::BINARY)) {
                        std::cerr << "Error writing serialization of the secret key to "
                                << party.sk
                                << std::endl;
                        return 1;
                }
                std::cout << "The secret key for party " << party.id << " has been generated and serialized" << std::endl;

                keyPairs.push_back(kp);
        }

        // Generate evalmult key

        // Generate evalsum key

        if (!Serial::SerializeToFile(config.cryptocontext, cryptoContext, SerType::BINARY)) {
                std::cerr << "Error writing serialization of the crypto context to "
                        << config.cryptocontext
                        << std::endl;
                return 1;
        }
        std::cout << "The cryptocontext has been serialized." << std::endl;

        std::ofstream multKeyFile("test/multkey.txt", std::ios::out | std::ios::binary);
        if (multKeyFile.is_open()) {
                if (!cryptoContext->SerializeEvalMultKey(multKeyFile, SerType::BINARY)) {
                        std::cerr << "Error writing eval mult keys" << std::endl;
                        std::exit(1);
                }
                std::cout << "EvalMult/ relinearization keys have been serialized" << std::endl;
                multKeyFile.close();
        }
        else {
                std::cerr << "Error serializing EvalMult keys" << std::endl;
                std::exit(1);
        }

        std::ofstream rotationKeyFile("test/rotkey.txt", std::ios::out | std::ios::binary);
        if (rotationKeyFile.is_open()) {
                if (!cryptoContext->SerializeEvalAutomorphismKey(rotationKeyFile, SerType::BINARY)) {
                        std::cerr << "Error writing rotation keys" << std::endl;
                        std::exit(1);
                }
                std::cout << "Rotation keys have been serialized" << std::endl;
        }
        else {
                std::cerr << "Error serializing Rotation keys" << std::endl;
                std::exit(1);
        }

        if (!Serial::SerializeToFile(config.pk, keyPairs.back().publicKey, SerType::BINARY)) {
                std::cerr << "Error writing serialization of the joined public key to "
                        << config.pk
                        << std::endl;
                return 1;
        }
        std::cout << "The joined public key has been serialized" << std::endl;
}
