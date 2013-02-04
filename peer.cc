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

// -----------------------------------------------------------------
// CONNECTIONS MANAGEMENT
// -----------------------------------------------------------------

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

void Peer::createLongDistanceLinks(Peer* lookupResult = NULL, bool timeoutError = false){

    #ifdef DEBUG_CREATELONGLINK
            ev << "DEBUG_CREATELONGLINK: " << "Inizio createLongDistanceLinks. Attempt = " << createLongDistanceLinks_attempts << endl;
    #endif

    if(timeoutError){
        //We are called, but after a timeout. We try again, doing another attempt, with another random id.
        createLongDistanceLinks_attempts++;
        createLongDistanceLinks_rndId = -1;
    }

    //We must create k long distance links, no more.
    if ((gateSize("longDistanceLink") >= (int)par("k")) || (createLongDistanceLinks_attempts >= (int)par("attemptsUpperBound"))){
        createLongDistanceLinks_rndId = -1;
        createLongDistanceLinks_attempts = 0;
        return;
    }

    // This create a random id, we'll connect to the manager of it. (see the paper for the formula)
    if (createLongDistanceLinks_rndId == -1) createLongDistanceLinks_rndId = exp(log(n) * (drand48() - 1.0));

    #ifdef DEBUG_CREATELONGLINK
        ev << "DEBUG_CREATELONGLINK: " << "Attempt (n. " << createLongDistanceLinks_attempts << ") di creazione con id: " << createLongDistanceLinks_rndId << endl;
    #endif

    // Check if we are the manager of the random id, or if we are already connected with the random id
    /* Note that, if we are called after a lookup result, we must do this checks again, because
     * the manager of the random id could connect with us while we was waiting the lookup result.
     */
    bool alreadyConnected = false;
    if(isManagerOf(createLongDistanceLinks_rndId)){
        alreadyConnected = true;
    }else{
        // Iterate all *output* links to find the manager of rndId
        cGate* gate;
        for (cModule::GateIterator i(this); !i.end(); i++) {
           gate = i();
           if (gate->getType() == cGate::OUTPUT) {
               if (gate->isConnected()) {
                   Peer* neighbor = dynamic_cast<Peer*>(gate->getNextGate()->getOwnerModule());

                   if (neighbor->isManagerOf(createLongDistanceLinks_rndId)) {
                       alreadyConnected = true;
                       break;
                   }
              }
           }
        }
    }

    if (!alreadyConnected) {

        /* If we don't have a member of rndId connected with us, and we aren't called
         * after a lookup response,we must call the lookup
         */
        if (lookupResult == NULL) {

            #ifdef DEBUG_CREATELONGLINK
                ev << "DEBUG_CREATELONGLINK: " << "chiamata alla richiesta di lookup per id: " << createLongDistanceLinks_rndId << endl;
            #endif

            requestLookup(createLongDistanceLinks_rndId, &Peer::createLongDistanceLinks);
            return;
        }

        //try to connect to the manager, if we can't connect to this node, we'll increase the attempts to try again
        if (connectTo(lookupResult, longDistanceLink)) {
            createLongDistanceLinks_attempts = 0;
        } else {
            createLongDistanceLinks_attempts++;
            #ifdef DEBUG_CREATELONGLINK
                ev << "DEBUG_CREATELONGLINK: " << "connect fallita" << endl;
            #endif
        }

    }

    //We'll try with another id
    createLongDistanceLinks_rndId = -1;
    scheduleAt(simTime() + uniform(0,0.01), new cMessage("createLongDistanceLinks"));

}


// -----------------------------------------------------------------
// JOIN
// -----------------------------------------------------------------
void joinNetwork(Peer *knownPeer) {
    double x = uniform(0,1);  // TODO: sicuro che non e' mai 1?
    //requestLookup(x, joinNetwork_Callback);
}
void joinNetwork_Callback(double x, Peer *manager) {

}


// -----------------------------------------------------------------
// LOOKUP
// -----------------------------------------------------------------

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
                    throw -1;
                else
                    test = fmin(fmin(abs(x - neighbor->id), abs(x - neighbor->id -1)), abs(x - neighbor->id +1));

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

/* TODO:
 * testare il timeout nel caso arrivi prima il timeout e poi una lookup che ha impiegato moltissimo tempo per far arrivare una risposta
 */
void Peer::requestLookup(double x, lookupCallbackPointer callback) {
    assert (!isManagerOf(x));

    unsigned long requestID = ++lookup_requestIDInc; //TODO verificare se fa errori di overflow o ricomincia da 0
    assert (requestID > 0);

    PendingLookup pl;
    pl.key = x;
    pl.callback = callback;
    pendingLookupRequests->insert(pair<unsigned long, PendingLookup>(requestID, pl));

    LookupMsg* msg = new LookupMsg();
    msg->setX(x);
    msg->setSenderID(getId());
    msg->setRequestID(requestID);
    msg->setHops(0);
    send(msg, getNextHopForKey(x).second);

    //Timeout
    LookupResponseMsg* msgTimeout = new LookupResponseMsg();
    msgTimeout->setX(x);
    msgTimeout->setManagerID(0);
    msgTimeout->setError(true);
    msgTimeout->setRequestID(requestID);
    scheduleAt(simTime() + getParentModule()->par("lookupTimeout"), msgTimeout);
}

// -----------------------------------------------------------------
// UTILITY
// -----------------------------------------------------------------

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

Peer* Peer::getPrevNeighbor() {
   return dynamic_cast<Peer*>(gate("shortLink$o", 0)->getNextGate()->getOwnerModule());
}

Peer* Peer::getNextNeighbor() {
   return dynamic_cast<Peer*>(gate("shortLink$o", 1)->getNextGate()->getOwnerModule());
}

// -----------------------------------------------------------------
// GRAPHICS
// -----------------------------------------------------------------

void Peer::updateDisplay() {
    char buf[64];
    sprintf(buf, "]%lf,%lf]", getPrevNeighbor()->id, id);
    getDisplayString().setTagArg("t", 0, buf);
}

// -----------------------------------------------------------------
// INITIALIZATION
// -----------------------------------------------------------------

void Peer::longDistanceLinksInitialization(){

    int attempts = 0;

    //We must create k long distance links, no more.
    if (gateSize("longDistanceLink") >= (int)par("k")) return;

    while (attempts < (int)par("attemptsUpperBound")){

        // see the paper for this formula
        double rndId = exp(log(n) * (drand48() - 1.0));

        Peer* neighbor = NULL;
        //find the manager of the random number, in the God's way :)
        for (cModule::SubmoduleIterator i(getParentModule()); !i.end(); i++) {
           if (strcmp(i()->getName(), getName()) == 0){
               Peer* peer = dynamic_cast<Peer*>(i());
               if (peer->isManagerOf(rndId)) {
                   neighbor = peer;
                   break;
               }
           }
        }

        //If we can't find a manager, we try with another number
        if (neighbor == NULL) {
            attempts++;
            continue;
        }

        //try to connect to the manager, if we can't connect to this node, increase the attempts
        if (connectTo(neighbor, longDistanceLink)) {
            scheduleAt(simTime() + uniform(0,0.01), new cMessage("longDistanceLinksInitialization"));
            return;
        } else {
           attempts++;
        }

    }

}

void Peer::initialize() {

    //If I am a member of a static network we initialize the connections at once.
    if (par("isStatic").boolValue()){
        //Estimation of n for the STATIC network (remember that, in this case, n is accurate. It's static!)
        n = (int)getParentModule()->par("n_static");

        //ID initialization for the STATIC network
        id = (double)par("id");

        //Short Link Creation for the STATIC network
        /* We create it in the ned file, but for completeness this is the code */
        /* Ci servirà quando i nodi dinamici si dovranno connettere */
        /* Peer* nextBrother = dynamic_cast<Peer*>(getParentModule()->getSubmodule(getName(), (getIndex() + 1) % n));
         connectTo(nextBrother, shortLink | shortLinkSucc);
         nextBrother->connectTo(this, shortLink | shortLinkPrev); */

        /* Long Distance Link Creation for the STATIC network
         * We need that all the initialization is done. This can be done with scheduleAt */
        //scheduleAt(simTime() + uniform(0,0.01), new cMessage("longDistanceLinksInitialization"));
        scheduleAt(simTime() + uniform(0,0.01), new cMessage("createLongDistanceLinks"));
    }

    pendingLookupRequests = new map<unsigned long, PendingLookup>();
    lookup_requestIDInc = 0;
    createLongDistanceLinks_rndId = -1;
    createLongDistanceLinks_attempts = 0;

    scheduleAt(simTime() + 12, new cMessage("debug"));


}

// -----------------------------------------------------------------
// MESSAGE MANAGEMENT
// -----------------------------------------------------------------

void Peer::handleMessage(cMessage *msg) {

    if (msg->isName("debug")) {
        //if (!isManagerOf(0.5)) requestLookup(0.5, callback_type_join, ...);
        delete msg;
    }

    else if (msg->isName("longDistanceLinksInitialization")) {
        longDistanceLinksInitialization();
        updateDisplay();
        delete msg;
    }

    else if (msg->isName("createLongDistanceLinks")) {
        createLongDistanceLinks();
        updateDisplay();
        delete msg;
    }

    else if (typeid(*msg) == typeid(LookupMsg)) {
        LookupMsg* luMsg = check_and_cast<LookupMsg*>(msg);
        double x = luMsg->getX();

        #ifdef DEBUG_LOOKUP
            ev << "DEBUG_LOOKUP: " << "ricevuto messaggio di lookup" << endl;
        #endif

        /* It forwards a lookup message for the key x, if the current Peer
         * is not the manager for x.
         * Otherwise, it contacts the original Peer who initiated the
         * first lookup request. */
        if (isManagerOf(x)) {

            #ifdef DEBUG_LOOKUP
                ev << "DEBUG_LOOKUP: " << "io sono il manager del messaggio di lookup, mando la risposta" << endl;
            #endif

            Peer* sender = dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(luMsg->getSenderID())); //TODO: Controllare il caso in cui getActiveSimulation()->getModule non fallisca
            LookupResponseMsg* lrMsg = new LookupResponseMsg();
            lrMsg->setManagerID(getId());
            lrMsg->setX(x);
            lrMsg->setRequestID(luMsg->getRequestID());
            lrMsg->setHops(luMsg->getHops());
            sendDirect(lrMsg, sender, "directin");

            delete msg;
        } else {

            #ifdef DEBUG_LOOKUP
                ev << "DEBUG_LOOKUP: " << "faccio il forward del lookup, requestID: " << luMsg->getRequestID() << endl;
            #endif

            luMsg->setHops(luMsg->getHops() + 1);
            send(luMsg, getNextHopForKey(x).second);
        }
    }

    else if (typeid(*msg) == typeid(LookupResponseMsg)) {

        LookupResponseMsg* mMsg = check_and_cast<LookupResponseMsg*>(msg);
        int requestID = mMsg->getRequestID();

        #ifdef DEBUG_LOOKUP
            if(mMsg->getError()){
                ev << "DEBUG_LOOKUP: " << "ricevuto timeout di lookup, requestID: " << mMsg->getRequestID() << endl;
            } else {
                ev << "DEBUG_LOOKUP: " << "ricevuto messaggio di response del lookup, requestID: " << mMsg->getRequestID() << endl;
            }
        #endif

        map<unsigned long, PendingLookup>::iterator it = pendingLookupRequests->find(requestID);
        if (it != pendingLookupRequests->end()){
            PendingLookup pl = it->second;
            pendingLookupRequests->erase(it);

            #ifdef DEBUG_LOOKUP
               ev << "DEBUG_LOOKUP: " << "chiamata la funzione di callback per la risposta di lookup, requestID: " << mMsg->getRequestID() << endl;
           #endif

            //TODO: Controllare il caso in cui getActiveSimulation()->getModule non fallisca
            Peer* manager = mMsg->getError() ? NULL : dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(mMsg->getManagerID()));
            (this->*pl.callback)(manager, mMsg->getError());
        }

        delete msg;

    }

}
