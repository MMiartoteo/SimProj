//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "peer.h"
#include <string>
#include <sstream>
using namespace std;

Define_Module(Peer);

bool Peer::connect(int peerIdFrom, int peerIdTo, LinkType linkType){

    cModule* pFrom = cSimulation::getActiveSimulation()->getModule(peerIdFrom); //getParentModule()->getSubmodule(getName(), peerIdFrom);
    cModule* pTo = cSimulation::getActiveSimulation()->getModule(peerIdTo); //getParentModule()->getSubmodule(getName(), peerIdTo);

    //Channel creation
    cChannelType *channelType = cChannelType::get((linkType == longDistanceLink) ? "symphony.LongDistanceLinkChannel" : "symphony.ShortLinkChannel");
    ostringstream channelNameStream;
    channelNameStream << ((linkType == longDistanceLink) ? "channelLong_" : "channelShort_") << peerIdFrom << "_" << peerIdTo;
    string channelName = channelNameStream.str();
    cChannel *channel = channelType->create(channelName.c_str());

    if (linkType == longDistanceLink) {
        //Every node maintains k long distance links, if the limit is reached, no other links can be created.
        if (pFrom->gateSize("longDistanceLinkOut") >= (int)par("k")) return false;
    }else{
        //Every node maintains 2 short links, if the limit is reached, no other links can be created.
        if (pFrom->gateSize("shortLinkOut") >= 2) return false;
    }

    //The number of incoming links per node is bounded by the upper limit of 2k.
    if (pTo->gateSize("longDistanceLinkIn") + pTo->gateSize("shortLinkIn") >= 2 * (int)par("k")) return false;

    //check if the two peer is already connected
    if(areConnected(peerIdFrom, peerIdTo)) return false;

    //Connect the nodes
    const char* outGateName = (linkType == longDistanceLink) ? "longDistanceLinkOut" : "shortLinkOut";
    const char* inGateName = (linkType == longDistanceLink) ? "longDistanceLinkIn" : "shortLinkIn";
    pFrom->setGateSize(outGateName, pFrom->gateSize(outGateName) + 1);
    pTo->setGateSize(inGateName, pTo->gateSize(inGateName) + 1);
    pFrom->gate(outGateName, pFrom->gateSize(outGateName) - 1)->connectTo(pTo->gate(inGateName, pTo->gateSize(inGateName) - 1), channel, false);

    //Initialize the channel
    channel->callInitialize();

    return true;
}

bool Peer::connectTo(int peerIdTo, LinkType linkType){
    return connect(getId(), peerIdTo, linkType);
}

bool Peer::connectFrom(int peerIdFrom, LinkType linkType){
    return connect(peerIdFrom, getId(), linkType);
}

bool Peer::disconnect(int peerIdFrom, int peerIdTo, LinkType linkType){
    if (!areConnected(peerIdFrom, peerIdTo)) return false;

    cModule* pFrom = cSimulation::getActiveSimulation()->getModule(peerIdFrom);
    cModule* pTo = cSimulation::getActiveSimulation()->getModule(peerIdTo);

    /* TODO
     * Se si trova un gate che collega pFrom con pTo si distrugge il gate (vedere bene documentazione, per comprendere i side effect)
     * */
}

bool Peer::disconnectLinkTo(int peerIdTo, LinkType linkType){
    return disconnect(getId(), peerIdTo, linkType);
}

bool Peer::disconnectLinkFrom(int peerIdFrom, LinkType linkType){
    return disconnect(peerIdFrom, getId(), linkType);
}

bool Peer::areConnected(int peerIdFrom, int peerIdTo){

    if (peerIdFrom == peerIdTo) return true;

    cModule* pFrom = cSimulation::getActiveSimulation()->getModule(peerIdFrom);

    cGate* gate;
    for (cModule::GateIterator i(pFrom); !i.end(); i++) {
        gate = i();

        //We analize only the output shortlink and the long distance link
        if ((strcmp(gate->getBaseName(), "shortLinkOut") == 0) || (strcmp(gate->getBaseName(), "longDistanceLinkOut") == 0)){
            //If the gate is connected with the pTo, it return true
            if(gate->getNextGate()->getOwnerModule()->getId() == peerIdTo) return true;
        }
    }
    return false;
}

bool Peer::isConnectedTo(int peerIdTo){
    return areConnected(getId(), peerIdTo);
}

bool Peer::isConnectedFrom(int peerIdFrom){
    return areConnected(peerIdFrom, getId());
}

void Peer::updateDisplay(){
    char buf[64];
    sprintf(buf, "%lf", id);
    getDisplayString().setTagArg("t", 0, buf);
}

void Peer::peerInizializationForStaticNetwork(){

    //Extimation of n for the STATIC network (remember that, in this case, n is accurate. It's static!)
    n = (int)getParentModule()->par("n");

    //ID inizialization for the STATIC network
    id = (double)par("id");

    //Short Link Creation for the STATIC network
    connectTo(getParentModule()->getSubmodule(getName(), (getIndex() + 1) % n)->getId(), shortLink);
    connectFrom(getParentModule()->getSubmodule(getName(), (getIndex() + 1) % n)->getId(), shortLink);

    //Long Distance Link Creation for the STATIC network
    //EXAMPLE (per generare i long distance link bisogna prendere come id uno casuale come descritto sul paper)
    // connectTo(getParentModule()->getSubmodule(getName(), (getIndex() * 2) % numPeersInNetwork)->getId(), longDistanceLink);
    // connectFrom(getParentModule()->getSubmodule(getName(), (getIndex() * 2) % numPeersInNetwork)->getId(), longDistanceLink);

    /* TODO: Long Distance Link Creation for the STATIC network
    *
    * Per fare la generazione dei long distance link in maniera corretta è necessario
    *
    * - Deve tirare a caso il numero (come descritto sul paper), questo è compreso tra 0 e 1.
    * - Deve trovare il peer che lo amministra (lo facciamo a bocce ferme, quindi enumerando tutti i nodi della rete, senza mandare messaggi)
    *
    *      for (cModule::SubmoduleIterator i(getParentModule()); !i.end(); i++) {
    *          cModule *peer = i();
    *          ...
    *      }
    *
    * - Collegarsi a questo. RITENTARE nel caso rifiuti, poichè ad esempio ha un numero troppo alto di link entranti.
    * - Bisogna tenere conto del parametro unidirectional della rete, per decidere se creare anche il long distance link inverso
    * - Questo va fatto per k volte
    *
    * */
}

void Peer::initialize() {

    //If I am a member of a static network we initialize the connections at once.
    if (par("isMemberOfAStaticNetwork").boolValue()) peerInizializationForStaticNetwork();

    updateDisplay();
}

void Peer::handleMessage(cMessage *msg) {

}
