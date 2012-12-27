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

bool Peer::connectLongDistanceLink(int peerIndexFrom, int peerIndexTo){
    cChannelType *channelType = cChannelType::get("symphony.StaticNetwork.LongDistanceLinkChannel");

    /* channel creation */
    ostringstream channelNameStream;
    channelNameStream << "channelLong_" << peerIndexFrom << "_" << peerIndexTo;
    string channelName = channelNameStream.str();
    cChannel *channel = channelType->create(channelName.c_str());

    cModule* pFrom = getParentModule()->getSubmodule(getName(), peerIndexFrom);
    cModule* pTo = getParentModule()->getSubmodule(getName(), peerIndexTo);

    int pFromLdlOGates = pFrom->gateSize("longDistanceLinkOut");
    int pToLdlIGates = pTo->gateSize("longDistanceLinkIn");
    int pToSlIGates = pTo->gateSize("shortLinkIn");

    //Every node maintains k long distance links, if the limit is reached, no other links can be created.
    if (pFromLdlOGates >= (int)par("k")) return false;

    //The number of incoming links per node is bounded by the upper limit of 2k.
    if (pToLdlIGates + pToSlIGates >= 2 * (int)par("k")) return false;

    //check if the two peer is already connected
    if(areConnected(peerIndexFrom, peerIndexTo)) return false;

    pFrom->setGateSize("longDistanceLinkOut", pFrom->gateSize("longDistanceLinkOut") + 1);
    pTo->setGateSize("longDistanceLinkIn", pTo->gateSize("longDistanceLinkIn") + 1);

    pFrom->gate("longDistanceLinkOut", pFromLdlOGates)->connectTo(pTo->gate("longDistanceLinkIn", pToLdlIGates), channel, false);
    channel->callInitialize();

    return true;
}

bool Peer::connectLongDistanceLinkTo(int peerIndexTo){
    return connectLongDistanceLink(getIndex(), peerIndexTo);
}

bool Peer::connectLongDistanceLinkFrom(int peerIndexFrom){
    return connectLongDistanceLink(peerIndexFrom, getIndex());
}

bool Peer::areConnected(int peerIndexFrom, int peerIndexTo){
    cModule* pFrom = getParentModule()->getSubmodule(getName(), peerIndexFrom);
    for (cModule::GateIterator i(pFrom); !i.end(); i++) {
        cGate *gate = i();
        if ((strcmp(gate->getBaseName(), "shortLinkOut") == 0) || (strcmp(gate->getBaseName(), "longDistanceLinkOut") == 0)){
            //ev << "NEXT GATE(" << gate->getFullName() << ") INDEX: " << gate->getNextGate()->getOwnerModule()->getIndex() << endl;
            if(gate->getNextGate()->getOwnerModule()->getIndex() == peerIndexTo) return true;
        }
    }
    return false;
}

bool Peer::isConnectedTo(int peerIndexTo){
    return areConnected(getIndex(), peerIndexTo);
}

bool Peer::isConnectedFrom(int peerIndexFrom){
    return areConnected(peerIndexFrom, getIndex());
}

void Peer::initialize() {

    connectLongDistanceLink(getIndex(), (getIndex() * 2) % (int)getParentModule()->par("numNodes"));
    //connectLongDistanceLink((getIndex() * 2) % (int)getParentModule()->par("numNodes"), getIndex());

}

void Peer::handleMessage(cMessage *msg) {

}
