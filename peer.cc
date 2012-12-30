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
#include <cassert>

using namespace std;

Define_Module(Peer);

bool Peer::connect(Peer* pFrom, Peer* pTo, long linkType) {

    //The number of incoming links per node is bounded by the upper limit of 2k.
    if (pTo->gateSize("longDistanceLinkIn") + pTo->gateSize("shortLinkIn") >= 2 * (int)par("k")) return false;

    //check if the two peer is already connected
    if (areConnected(pFrom, pTo)) return false;

    //Channel creation
    cChannelType *channelType = cChannelType::get((linkType & longDistanceLink) ? "symphony.LongDistanceLinkChannel" : "symphony.ShortLinkChannel");
    ostringstream channelNameStream;
    channelNameStream << ((linkType & longDistanceLink) ? "channelLong_" : "channelShort_") << pFrom->id << "_" << pTo->id;
    string channelName = channelNameStream.str();
    cChannel *channel = channelType->create(channelName.c_str());

    //Connect the nodes
    if  (linkType == longDistanceLink) {
        pFrom->setGateSize("longDistanceLinkOut", pFrom->gateSize("longDistanceLinkOut") + 1);
        pTo->setGateSize("longDistanceLinkIn", pTo->gateSize("longDistanceLinkIn") + 1);
        pFrom->gate("longDistanceLinkOut", pFrom->gateSize("longDistanceLinkOut") - 1)->connectTo(
                pTo->gate("longDistanceLinkIn", pTo->gateSize("longDistanceLinkIn") - 1), channel, false);
    } else {
        if (linkType & shortLinkSucc) {
            pFrom->gate("shortLinkOut", 1)->connectTo(pTo->gate("shortLinkIn", 0), channel, false);
        } else {
            pFrom->gate("shortLinkOut", 0)->connectTo(pTo->gate("shortLinkIn", 1), channel, false);
        }
    }

    //Initialize the channel
    channel->callInitialize();

    return true;
}

bool Peer::connectTo(Peer* pTo, long linkType) {
    return connect(this, pTo, linkType);
}

bool Peer::connectFrom(Peer* pFrom, long linkType) {
    return connect(pFrom, this, linkType);
}

bool Peer::disconnect(Peer* pFrom, Peer* pTo) {
    if (!areConnected(pFrom, pTo)) return false;

    /* TODO
     * Se si trova un gate che collega pFrom con pTo si distrugge il gate (vedere bene documentazione, per comprendere i side effect)
     * */
    return false;
}

bool Peer::disconnectLinkTo(Peer* pTo) {
    return disconnect(this, pTo);
}

bool Peer::disconnectLinkFrom(Peer* pFrom) {
    return disconnect(pFrom, this);
}

bool Peer::areConnected(Peer* pFrom, Peer* pTo) {

    if (pFrom == pTo) return true;

    cGate* gate;
    for (cModule::GateIterator i(pFrom); !i.end(); i++) {
        gate = i();

        //We analize only the output shortlink and the long distance link
        if ((strcmp(gate->getBaseName(), "shortLinkOut") == 0) || (strcmp(gate->getBaseName(), "longDistanceLinkOut") == 0)) {
            //If the gate is connected with the pTo, it return true
            if (gate->isConnected()) {
                if(gate->getNextGate()->getOwnerModule() == pTo) return true;
            }
        }
    }
    return false;
}

bool Peer::isConnectedTo(Peer* pTo) {
    return areConnected(this, pTo);
}

bool Peer::isConnectedFrom(Peer* pFrom) {
    return areConnected(pFrom, this);
}

Peer* Peer::getPrevNeighbor() {
   return dynamic_cast<Peer*>(gate("shortLinkOut", 0)->getNextGate()->getOwnerModule());
}

Peer* Peer::getNextNeighbor() {
   return dynamic_cast<Peer*>(gate("shortLinkOut", 1)->getNextGate()->getOwnerModule());
}

bool Peer::isManagerOf(double x) {
    Peer* previous = getPrevNeighbor();

    // case 0: THIS == X
    if (x == id) return true;

    // case 1: |--..--PREVIOUS--X--THIS--...--|
    if ((previous->id < x) && (x < id)) return true;

    // case 2: |--X--THIS--...--PREVIOUS--|
    if ((x < id) && (id < previous->id)) return true;

    // case 3: |--THIS--...--PREVIOUS--X--|
    if ((x > previous->id) && (id < previous->id)) return true;

    return false;
}

void Peer::updateDisplay() {
    char buf[64];
    sprintf(buf, "%lf", id);
    getDisplayString().setTagArg("t", 0, buf);
}

void Peer::peerInizializationForStaticNetwork() {

    ev << "DEBUG: peerInizializationForStaticNetwork" << endl;

    //Extimation of n for the STATIC network (remember that, in this case, n is accurate. It's static!)
    n = (int)getParentModule()->par("n");

    //ID inizialization for the STATIC network
    id = (double)par("id");

    //Short Link Creation for the STATIC network
    connectTo(dynamic_cast<Peer*>(getParentModule()->getSubmodule(getName(), (getIndex() + 1) % n)), shortLink | shortLinkSucc);
    connectFrom(dynamic_cast<Peer*>(getParentModule()->getSubmodule(getName(), (getIndex() + 1) % n)), shortLink | shortLinkPrev);

    //Long Distance Link Creation for the STATIC network
    //EXAMPLE (per generare i long distance link bisogna prendere come id uno casuale come descritto sul paper)
    // connectTo(getParentModule()->getSubmodule(getName(), (getIndex() * 2) % numPeersInNetwork)->getId(), longDistanceLink);
    // connectFrom(getParentModule()->getSubmodule(getName(), (getIndex() * 2) % numPeersInNetwork)->getId(), longDistanceLink);

    /* TODO: Long Distance Link Creation for the STATIC network
    *
    * Per fare la generazione dei long distance link in maniera corretta � necessario
    *
    * - Deve tirare a caso il numero (come descritto sul paper), questo � compreso tra 0 e 1.
    * - Deve trovare il peer che lo amministra (lo facciamo a bocce ferme, quindi enumerando tutti i nodi della rete, senza mandare messaggi)
    *
    *      for (cModule::SubmoduleIterator i(getParentModule()); !i.end(); i++) {
    *          cModule *peer = i();
    *          ...
    *      }
    *
    * - Collegarsi a questo. RITENTARE nel caso rifiuti, poich� ad esempio ha un numero troppo alto di link entranti.
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
