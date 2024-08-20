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

        // Run each party sequentially
        for ( auto& party: config.parties ) {
                std::cout << "Running party " << party.id << std::endl;

                // Deserialize the cryptocontext
                CryptoContext<DCRTPoly> cryptoContext;
                if (!Serial::DeserializeFromFile(config.cryptocontext, cryptoContext, SerType::BINARY)) {
                        std::cerr << "I cannot read serialization from " << config.cryptocontext << std::endl;
                        return 1;
                }
                std::cout << "The cryptocontext has been deserialized." << std::endl;

                std::ifstream multKeyIStream("test/multkey.txt", std::ios::in | std::ios::binary);
                if (!multKeyIStream.is_open()) {
                        std::cerr << "Cannot read serialization from " << std::endl;
                        std::exit(1);
                }
                if (!cryptoContext->DeserializeEvalMultKey(multKeyIStream, SerType::BINARY)) {
                        std::cerr << "Could not deserialize eval mult key file" << std::endl;
                        std::exit(1);
                }

                std::cout << "Deserialized eval mult keys" << '\n' << std::endl;
                std::ifstream rotKeyIStream("test/rotkey.txt", std::ios::in | std::ios::binary);
                if (!rotKeyIStream.is_open()) {
                        std::cerr << "Cannot read serialization from " << std::endl;
                        std::exit(1);
                }
                if (!cryptoContext->DeserializeEvalAutomorphismKey(rotKeyIStream, SerType::BINARY)) {
                        std::cerr << "Could not deserialize eval rot key file" << std::endl;
                        std::exit(1);
                }

                PublicKey<DCRTPoly> pk;
                if (Serial::DeserializeFromFile(config.pk, pk, SerType::BINARY) == false) {
                        std::cerr << "Could not read public key" << std::endl;
                        return 1;
                }
                std::cout << "The public key has been deserialized." << std::endl;

                Plaintext pt = cryptoContext->MakePackedPlaintext(party.values);
                Ciphertext<DCRTPoly> ct = cryptoContext->Encrypt(pk, pt);

                std::cout << "The plaintexts have been encrypted." << std::endl;

                if (!Serial::SerializeToFile(party.ct, ct, SerType::BINARY)) {
                        std::cerr << "Error writing serialization of ciphertext to " << party.ct << std::endl;
                        return 1;
                }
                std::cout << "The ciphertext has been serialized." << std::endl;
        }
}
