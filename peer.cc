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
#include "Msgs_m.h"
#include <string>
#include <sstream>
#include <cassert>
#include <cmath>
#include <assert.h>

using namespace std;

Define_Module(Peer);

bool Peer::connect(Peer* pFrom, Peer* pTo, long linkType) {

    //The number of incoming links per node is bounded by the upper limit of 2k.
    if (pTo->gateSize("longDistanceLink") >= 2 * (int)par("k")) return false;

    //check if the two peer is already connected
    if (areConnected(pFrom, pTo)) return false;

    //Channel creation
    cChannelType *channelType = cChannelType::get((linkType & longDistanceLink) ? "symphony.LongDistanceLinkChannel" : "symphony.ShortLinkChannel");
    ostringstream channelNameStream;
    channelNameStream << ((linkType & longDistanceLink) ? "channelLong_" : "channelShort_") << pFrom->id << "_" << pTo->id;
    cChannel *channel = channelType->create(channelNameStream.str().c_str());
    channelNameStream.clear();
    channelNameStream << ((linkType & longDistanceLink) ? "channelLong_" : "channelShort_") << pTo->id << "_" << pFrom->id;
    cChannel *channelRev = channelType->create(channelNameStream.str().c_str());

    //Connect the nodes
    if  (linkType == longDistanceLink) {
        cGate *pFromLdli, *pFromLdlo, *pToLdli, *pToLdlo;
        pFrom->getOrCreateFirstUnconnectedGatePair("longDistanceLink", false, true, pFromLdli, pFromLdlo);
        pTo->getOrCreateFirstUnconnectedGatePair("longDistanceLink", false, true, pToLdli, pToLdlo);
        pFromLdlo->connectTo(pToLdli, channel, false);
        if(!getParentModule()->par("unidirectional").boolValue()) {
            pToLdlo->connectTo(pFromLdli, channelRev, false);
        }
    } else {
        if (linkType & shortLinkSucc) {
            pFrom->gate("shortLink$o", 1)->connectTo(pTo->gate("shortLink$i", 0), channel, false);
            pTo->gate("shortLink$o", 0)->connectTo(pFrom->gate("shortLink$i", 1), channelRev, false);
        } else {
            pFrom->gate("shortLink$o", 0)->connectTo(pTo->gate("shortLink$i", 1), channel, false);
            pTo->gate("shortLink$o", 1)->connectTo(pFrom->gate("shortLink$i", 0), channelRev, false);
        }
    }

    return true;
}

bool Peer::connectTo(Peer* pTo, long linkType) {
    return connect(this, pTo, linkType);
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

pair<Peer*,cGate*> Peer::getNextHopForKey(double x) {
    assert (! this->isManagerOf(x));

    Peer* bestPeer = NULL;
    cGate* bestGate = NULL;
    double currBest = -1.0;

    cGate* gate;
    for (cModule::GateIterator i(this); !i.end(); i++) {
        gate = i();

        // Iterate all *output* links
        if (gate->getType() == cGate::OUTPUT) {
            if (gate->isConnected()) {
                Peer* neighbor = dynamic_cast<Peer*>(gate->getNextGate()->getOwnerModule());

                double test;
                if (getParentModule()->par("unidirectional").boolValue())
                    test = x - neighbor->id;
                else
                    test = abs(x - neighbor->id);

                if (currBest == -1.0 || (test >= 0  && test < currBest)) {
                    bestPeer = neighbor;
                    bestGate = gate;
                    currBest = test;
                }
            }
        }

    }

    return pair<Peer*,cGate*>(bestPeer, bestGate);
}

void Peer::requestLookup(double x, CallbackType c) {
    assert (!isManagerOf(x));

    PendingLookup pl;
    pl.key = x;
    pl.requestID = ++lookup_requestIDInc; //TODO verificare se fa errori di overflow o ricomincia da 0
    pl.callback = c;
    pendingLookupRequests->push_back(pl);

    LookupMsg* msg = new LookupMsg();
    msg->setX(x);
    msg->setSenderID(getId());
    msg->setRequestID(pl.requestID);
    msg->setHops(0);
    send(msg, getNextHopForKey(x).second);
}


bool Peer::areConnected(Peer* pFrom, Peer* pTo) {

    if (pFrom == pTo) return true;

    cGate* gate;
    for (cModule::GateIterator i(pFrom); !i.end(); i++) {
        gate = i();

        // We analyze only the output short links and the long distance links
        if (gate->getType() == cGate::OUTPUT){
            // If the gate is connected with the pTo, it return true
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

Peer* Peer::getPrevNeighbor() {
   return dynamic_cast<Peer*>(gate("shortLink$o", 0)->getNextGate()->getOwnerModule());
}

Peer* Peer::getNextNeighbor() {
   return dynamic_cast<Peer*>(gate("shortLink$o", 1)->getNextGate()->getOwnerModule());
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

double Peer::getSegmentLength() {
    Peer* previous = getPrevNeighbor();

    // case 0: THIS == X
    if (previous->id == id) return 0;

    // case 1: |--..--PREVIOUS++THIS--...--|
    if (previous->id < id) return id - previous->id;

    // case 2: |++THIS--...--PREVIOUS++|
    if (id < previous->id) return id + 1.0 - previous->id;

    return false;
}

void Peer::updateDisplay() {
    char buf[64];
    sprintf(buf, "%lf : %lf : %lf", id, getPrevNeighbor()->id, getSegmentLength());
    getDisplayString().setTagArg("t", 0, buf);
}

void Peer::peerInizializationForStaticNetwork() {

    //Estimation of n for the STATIC network (remember that, in this case, n is accurate. It's static!)
    n = (int)getParentModule()->par("n");

    //ID initialization for the STATIC network
    id = (double)par("id");

    //Short Link Creation for the STATIC network
    /* We create it in the ned file, but for completeness this is the code */
    /* Peer* nextBrother = dynamic_cast<Peer*>(getParentModule()->getSubmodule(getName(), (getIndex() + 1) % n));
     connectTo(nextBrother, shortLink | shortLinkSucc);
     nextBrother->connectTo(this, shortLink | shortLinkPrev); */

    /* Long Distance Link Creation for the STATIC network
     * We need that all the initialization is done. This can be done with scheduleAt */
    scheduleAt(simTime() + uniform(0,0.01), new cMessage("createLongDistanceLinksForStaticNetwork"));
    //createLongDistanceLinkForStaticNetwork();

}

void Peer::createLongDistanceLinkForStaticNetwork(){

    int attempts = 0;

    //We must create k long distance links, no more.
    if (gateSize("longDistanceLink") >= (int)par("k")) return;

    while (attempts < (int)par("attemptsUpperBound")){

        // see the paper for this formula
        double rndId = exp(log(n) * (drand48() - 1.0));

        //find the manager of the random number, in the God's way :)
        Peer* rndIdManager = NULL;
        for (cModule::SubmoduleIterator i(getParentModule()); !i.end(); i++) {
           if (strcmp(i()->getName(), getName()) == 0){
               Peer* peer = dynamic_cast<Peer*>(i());
               if (peer->isManagerOf(rndId)) {
                   rndIdManager = peer;
                   break;
               }
           }
        }

        //If we can't find a manager, we try with another number
        if (rndIdManager == NULL) {
            attempts++;
            continue;
        }

        //try to connect to the manager, if we can't connect to this node, increase the attempts
        if (connectTo(rndIdManager, longDistanceLink)) {
            scheduleAt(simTime() + uniform(0,0.01), new cMessage("createLongDistanceLinksForStaticNetwork"));
            return;
        } else {
           attempts++;
        }

    }


}

void Peer::initialize() {

    //If I am a member of a static network we initialize the connections at once.
    if (par("isMemberOfAStaticNetwork").boolValue()) peerInizializationForStaticNetwork();

    pendingLookupRequests = new list<PendingLookup>();
    lookup_requestIDInc = 0;

    scheduleAt(simTime() + 12, new cMessage("debug"));


}

void Peer::handleMessage(cMessage *msg) {

    if (msg->isName("debug")) {
        if (!isManagerOf(0.5)) requestLookup(0.5, join);
        delete msg;
    }

    else if (msg->isName("createLongDistanceLinksForStaticNetwork")) {
        createLongDistanceLinkForStaticNetwork();
        updateDisplay();
        delete msg;
    }

    else if (typeid(*msg) == typeid(LookupMsg)) {
        LookupMsg* luMsg = check_and_cast<LookupMsg*>(msg);
        double x = luMsg->getX();

        ev << "DEBUG: " << "ricevuto messaggio di lookup" << endl;

        /* It forwards a lookup message for the key x, if the current Peer
         * is not the manager for x.
         * Otherwise, it contacts the original Peer who initiated the
         * first lookup request. */
        if (isManagerOf(x)) {

            ev << "DEBUG: " << "io sono il manager del messaggio di lookup, mando la risposta" << endl;

            Peer* sender = dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(luMsg->getSenderID())); //TODO: Controllare il caso in cui getActiveSimulation()->getModule non fallisca
            LookupResponseMsg* lrMsg = new LookupResponseMsg();
            lrMsg->setManagerID(getId());
            lrMsg->setX(x);
            lrMsg->setRequestID(luMsg->getRequestID());
            lrMsg->setHops(luMsg->getHops());
            sendDirect(lrMsg, sender, "directin");

            delete msg;
        } else {

            ev << "DEBUG: " << "faccio il forward del lookup" << endl;

            luMsg->setHops(luMsg->getHops() + 1);
            send(luMsg, getNextHopForKey(x).second);
        }
    }

    else if (typeid(*msg) == typeid(LookupResponseMsg)) {

        ev << "DEBUG: " << "ricevuto messaggio di response del lookup" << endl;

        LookupResponseMsg* mMsg = check_and_cast<LookupResponseMsg*>(msg);
        int requestID = mMsg->getRequestID();

        for (list<PendingLookup>::iterator it = pendingLookupRequests->begin(); it != pendingLookupRequests->end(); it++){
            if (it->requestID == requestID) {

                //TODO si deve mettere da qualche parte il risultato
                Peer* manager = dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(mMsg->getManagerID())); //TODO: Controllare il caso in cui getActiveSimulation()->getModule non fallisca
                double x = mMsg->getX();
                //...

                //TODO eliminare elemento dalla lista

                switch(it->callback) {
                case join:
                    ev << "chiama join" << endl; //TODO: chiamare join
                    break;
                case longLinkCreation:
                    ev << "chiama creazione dei long link" << endl; //TODO: chiamare creazione dei long link
                    break;
                case query:
                    ev << "chiama query" << endl; //TODO: chiamare query
                }
            }
        }

        delete msg;

    }

}
