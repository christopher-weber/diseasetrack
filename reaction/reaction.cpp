#include "reaction.h"

Reaction::Reaction() {}

Reaction::~Reaction() {}

const std::string Reaction::identify() { return std::string("Reaction"); }

void Reaction::init() { return; }

void Reaction::pause() { return; }

void Reaction::run() { return; }

void Reaction::AcceptRunModeOrder(int _in) { return; }

void Reaction::AcceptClusters(std::tuple<std::string,std::vector<std::string>> _clusters){
  std::string sk = "YlK2GuJJ5qJX6sSyWoPM69QqWFIsy1BbjNh7S8sUfZE="; //WE KNOW THAT THIS DOESNT BELONG HERE!!!
  std::string spk = "VH0QivWjENisAcrZGSWkYFsgrcsdaAPuSiXdb9puyUM=";
  std::string skdebase = base64_decode(sk);
  std::string spkdebase = base64_decode(spk);
  std::string pk = base64_decode(std::get<0>(_clusters));
  //logMessage("Sender PK: " + std::get<0>(_clusters), "INFO");
  //logMessage("PK length in bytes is: " + std::to_string(pk.length()), "INFO");
  std::vector<std::string> cluster_to_tangle;
  cluster_to_tangle.clear();
  for(unsigned int i = 0; i < std::get<1>(_clusters).size(); i++){
    //logMessage("Encrypted: " + std::get<1>(_clusters).at(i), "INFO");
    std::string debase64 = base64_decode(std::get<1>(_clusters).at(i));
    //logMessage("Debased: " + debase64, "INFO");
    //logMessage("Cyphertext length in bytes is: " + std::to_string(debase64.length()), "INFO");
    unsigned char pkarr[256];
    std::copy( spkdebase.begin(), spkdebase.end(), pkarr );
    pkarr[spkdebase.length()] = 0;
    unsigned char skarr[256];
    std::copy( skdebase.begin(), skdebase.end(), skarr );
    skarr[skdebase.length()] = 0;
    //crypto_box_keypair(pkarr, skarr);
    unsigned char decrypted[debase64.length() - crypto_box_SEALBYTES];
    int here = crypto_box_seal_open(decrypted, reinterpret_cast<const unsigned char*>(debase64.c_str()), debase64.length(), pkarr, skarr);
    //std::cout << "HERE: " << here << std::endl;
    std::string result = "";
    for(unsigned int i = 0; i < debase64.length() - crypto_box_SEALBYTES; i++){
      result += decrypted[i];
    }
    cluster_to_tangle.push_back(result);
    logMessage(result, "INFO");
  }
  boost::property_tree::ptree message;
  boost::property_tree::ptree clusterarray;
  message.put("Command", "SendMessage");
  for(unsigned int i = 0; i < cluster_to_tangle.size(); i++){
    boost::property_tree::ptree temp;
    temp.put("", cluster_to_tangle.at(i));
    clusterarray.push_back(std::make_pair("", temp));
  }
  boost::property_tree::ptree ntruarray;
  
  boost::property_tree::ptree arraypart;
  arraypart.add_child("Clusters", clusterarray);
  message.add_child("MessageBody", arraypart);
  message.add_child("NTRUPubKeyArray", ntruarray);
  std::stringstream oss;
    write_json(oss, message);
    logMessage(oss.str(), "INFO");
    std::vector<std::string> ntrukeys;
    
    WC *wc = new WC();
    wc->pushMessage(oss.str(), ntrukeys);
    delete(wc);
  return;
}
