#pragma once

#include <jsoncpp/json/json.h>
#include <iostream>
#include <fstream>
#include <optional>

class PartyConfig {
        public:
                int port;
                std::string id;
                std::string sk;
                std::string ct;
                std::vector<long int> values;
                PartyConfig(int port, std::string id, std::string sk, std::string ct, std::vector<long int> values) : port(port), id(id), sk(sk), ct(ct), values(values) {}

                friend std::ostream& operator<<(std::ostream& os, const PartyConfig& party) {
                        os << "Port: " << party.port << " Id: " << party.id << "SK: " << party.sk << "CT: " << party.ct << " Values:";
                        for ( int value: party.values ) {
                                os << " " << value;
                        }
                        os << std::endl;
                        return os;
                }
};

class Config {
        public:
                std::vector<PartyConfig> parties;
                std::string cryptocontext;
                std::string pk;

                Config(std::string filePath) {
                        std::ifstream ifs;
                        ifs.open(filePath);

                        Json::Value root;
                        ifs >> root;

                        ifs.close();

                        const Json::Value partiesJson = root["parties"];
                        for ( unsigned int i = 0; i < partiesJson.size(); ++i ) {
                                auto port = partiesJson[i]["port"].asInt();
                                auto id = partiesJson[i]["id"].asString();
                                auto sk = partiesJson[i]["sk"].asString();
                                auto ct = partiesJson[i]["ct"].asString();
                                std::vector<long int> values;

                                for ( unsigned int j = 0; j < partiesJson[i]["values"].size(); ++j ) {
                                        values.push_back(partiesJson[i]["values"][j].asInt());
                                }

                                PartyConfig party = PartyConfig(port, id, sk, ct, values);

                                parties.push_back(party);
                        }

                        cryptocontext = root["cryptocontext"].asString();

                        pk = root["pk"].asString();
                }

                friend std::ostream& operator<<(std::ostream& os, const Config& config) {
                        os << "Parties:" << std::endl;
                        for (auto& party : config.parties) {
                                os << party;
                        }
                        os << std::endl;

                        os << "CryptoContext:" << std::endl;
                        os << config.cryptocontext;
                        os << std::endl;

                        os << "PK:" << std::endl;
                        os << config.pk;
                        os << std::endl;

                        return os;
                }
};
