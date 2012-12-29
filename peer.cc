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

bool Peer::connect(Peer* pFrom, Peer* pTo, LinkType linkType){

    //Channel creation
    cChannelType *channelType = cChannelType::get((linkType == longDistanceLink) ? "symphony.LongDistanceLinkChannel" : "symphony.ShortLinkChannel");
    ostringstream channelNameStream;
    channelNameStream << ((linkType == longDistanceLink) ? "channelLong_" : "channelShort_") << pFrom->id << "_" << pTo->id;
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
    if(areConnected(pFrom, pTo)) return false;

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

bool Peer::connectTo(Peer* pTo, LinkType linkType){
    return connect(this, pTo, linkType);
}

bool Peer::connectFrom(Peer* pFrom, LinkType linkType){
    return connect(pFrom, this, linkType);
}

bool Peer::disconnect(Peer* pFrom, Peer* pTo, LinkType linkType){
    if (!areConnected(pFrom, pTo)) return false;

    /* TODO
     * Se si trova un gate che collega pFrom con pTo si distrugge il gate (vedere bene documentazione, per comprendere i side effect)
     * */
}

bool Peer::disconnectLinkTo(Peer* pTo, LinkType linkType){
    return disconnect(this, pTo, linkType);
}

bool Peer::disconnectLinkFrom(Peer* pFrom, LinkType linkType){
    return disconnect(pFrom, this, linkType);
}

bool Peer::areConnected(Peer* pFrom, Peer* pTo){

    if (pFrom == pTo) return true;

    cGate* gate;
    for (cModule::GateIterator i(pFrom); !i.end(); i++) {
        gate = i();

        //We analize only the output shortlink and the long distance link
        if ((strcmp(gate->getBaseName(), "shortLinkOut") == 0) || (strcmp(gate->getBaseName(), "longDistanceLinkOut") == 0)){
            //If the gate is connected with the pTo, it return true
            if(gate->getNextGate()->getOwnerModule() == pTo) return true;
        }
    }
    return false;
}

bool Peer::isConnectedTo(Peer* pTo){
    return areConnected(this, pTo);
}

bool Peer::isConnectedFrom(Peer* pFrom){
    return areConnected(pFrom, this);
}

Peer* Peer::getPreviousNeighbor(){
   /* find the previous peer*/

    Peer* neighbor[2] = {
        dynamic_cast<Peer*>(gate("shortLinkOut", 0)->getNextGate()->getOwnerModule()),
        dynamic_cast<Peer*>(gate("shortLinkOut", 1)->getNextGate()->getOwnerModule())
    };

    // case 1: |-----N0----THIS-----N1----| or |-----N1----THIS-----N0----|
    if (neighbor[0]->id < id && id < neighbor[1]->id) return neighbor[0];
    if (neighbor[1]->id < id && id < neighbor[0]->id) return neighbor[1];

    // case 2: |--THIS---N0--N1----| or |--THIS---N1--N0----|
    if (neighbor[0]->id > id && neighbor[1]->id > id){
       //TODO /*...*/
    }

    // case 3: |-----N0--N1--THIS------| or |-----N1--N0--THIS------|
    if (neighbor[0]->id < id && neighbor[1]->id < id){
        //TODO /*...*/
    }

    /*...*/
    return this;

}

bool Peer::isManagerOf(double x) {
    Peer* previous = getPreviousNeighbor();

    //if (/*...*/) return true;
    //if (/*...*/) return true;

    return false;
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
    connectTo(dynamic_cast<Peer*>(getParentModule()->getSubmodule(getName(), (getIndex() + 1) % n)), shortLink);
    connectFrom(dynamic_cast<Peer*>(getParentModule()->getSubmodule(getName(), (getIndex() + 1) % n)), shortLink);

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
