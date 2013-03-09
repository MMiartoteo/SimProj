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

void Peer::connect(Peer* pFrom, Peer* pTo, long linkType) {

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

}

void Peer::connectTo(Peer* pTo, long linkType) {
    return connect(this, pTo, linkType);
}

bool Peer::disconnect(Peer* pFrom, Peer* pTo) {

    if (pFrom == pTo) return false;

    cGate* gate;
    bool found = false;
    for (cModule::GateIterator i(pFrom); !i.end(); i++) {
       gate = i();

       if (gate->getType() == cGate::OUTPUT){
           if (gate->isConnected()) {
               if(gate->getNextGate()->getOwnerModule() == pTo) {
                   if (gate->getNextGate()->isConnected()) gate->getNextGate()->disconnect();
                   gate->disconnect();
                   found = true; //we continue to resolve possible errors of the ring (when the others leave it)
               }
           }
       }

    }

    for (cModule::GateIterator i(pTo); !i.end(); i++) {
       gate = i();

       if (gate->getType() == cGate::OUTPUT){
           if (gate->isConnected()) {
               if(gate->getNextGate()->getOwnerModule() == pFrom) {
                   if (gate->getNextGate()->isConnected()) gate->getNextGate()->disconnect();
                   gate->disconnect();
                   found = true; //we continue to resolve possible errors of the ring (when the others leave it)
               }
           }
       }

    }

    return found;
}

bool Peer::disconnectLinksTo(Peer* pTo) {
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

unsigned int Peer::getNumberOfConnectedLongLinkGates() {
    int ris = 0;
    for (int i = 0; i < gateSize("longDistanceLink"); i++) {
        if (gateHalf("longDistanceLink", cGate::OUTPUT, i)->isConnected()) ris++;
    }
    return ris;
}

void Peer::createLongDistanceLinks(Peer* manager = NULL){

    if(manager == NULL){
        //We are called, but after a timeout. We try again, doing another attempt, with another random id.
        createLongDistanceLinks_attempts++;
        createLongDistanceLinks_rndId = -1;
    }

    #ifdef DEBUG_CREATELONGLINK
            ev << "DEBUG_CREATELONGLINK: " << "Inizio createLongDistanceLinks. Attempt = " << createLongDistanceLinks_attempts << endl;
    #endif

    // Check if the long-distance creation is terminated (k reached OR too many attempts)
    if ((getNumberOfConnectedLongLinkGates() >= (int)par("k")) || (createLongDistanceLinks_attempts >= (int)par("attemptsUpperBound"))){
        createLongDistanceLinks_rndId = -1;
        createLongDistanceLinks_attempts = -1;

        #ifdef DEBUG_CREATELONGLINK
            ev << "DEBUG_CREATELONGLINK: " << "Rinuncia creazione long link: attempts = " << createLongDistanceLinks_attempts << " numeroConnessioni = " << getNumberOfConnectedLongLinkGates() << endl;
       #endif

        // Either we were joining or re-linking, we are now connected anyways.
        state = Connected;

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
        if (manager == NULL) {

            #ifdef DEBUG_CREATELONGLINK
                ev << "DEBUG_CREATELONGLINK: " << "chiamata alla richiesta di lookup per id: " << createLongDistanceLinks_rndId << endl;
            #endif

            requestLookup(createLongDistanceLinks_rndId, &Peer::createLongDistanceLinks, lookupNoSpecialization);
            return;
        }

        //try to connect to the manager, if we can't connect to this node, we'll increase the attempts to try again
        if (manager->getNumberOfConnectedLongLinkGates() < 2 * (int)par("k")) { //The number of incoming links per node is bounded by the upper limit of 2k.
            connectTo(manager, longDistanceLink);
            createLongDistanceLinks_attempts = -1;
        } else {
            createLongDistanceLinks_attempts++;
            #ifdef DEBUG_CREATELONGLINK
                ev << "DEBUG_CREATELONGLINK: " << "troppi link in entrata" << endl;
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
void Peer::requestJoin(double x = -1) {
    state = Joining;
    joinRequestedId = (x == -1) ? uniform(0, 1) : x;

    #ifdef DEBUG_JOIN
            ev << "DEBUG_JOIN: " << "Request join. requested id: " << joinRequestedId << endl;
    #endif

    unsigned long requestID = ++lookup_requestIDInc;
    assert (requestID > 0);

    requestLookup(joinRequestedId, &Peer::requestJoinCallback, lookupJoinSpecialization);
}

void Peer::requestJoinCallback(Peer* manager) {
    #ifdef DEBUG_JOIN
        ev << "DEBUG_JOIN: " << "Join callback, now I need to create long links" << endl;
        ev << "DEBUG_JOIN: " << "Join callback, n estimation: " << n << endl;
    #endif

    if (manager != NULL) {
        createLongDistanceLinks();
    } else {
        joinFailuresForElapsedTimeout++;
        requestJoin(joinRequestedId);
    }
}

void Peer::join(Peer* joiningPeer, double requestedId) {

    #ifdef DEBUG_JOIN
        ev << "DEBUG_JOIN: " << "Join" << endl;
    #endif

    /*
     * Assume that we have sent a message for join to the previous,
     * and that we had waited its lock (this is not very correct because this require some time and messages).
     * Now we can do an atomic action because the three involved nodes are waiting for the join.
     * To do the atomic action we don't send any message, in this way omnet lets only us to run
     * */

    Peer* prevPeer = getPrevNeighbor();
    Peer* nextPeer = getNextNeighbor();

    //n - estimation
    double Xs = prevPeer->getSegmentLength() + getSegmentLength() + nextPeer->getSegmentLength();
    NEstimationMsg* m;
    m = new NEstimationMsg(); m->setN(3/Xs);
    sendDirect(m, nextPeer, "directin");
    m = new NEstimationMsg(); m->setN(3/Xs);
    sendDirect(m, prevPeer, "directin");
    m = new NEstimationMsg(); m->setN(3/Xs);
    scheduleAt(simTime(), m);
    joiningPeer->n = 3/Xs;

    disconnectLinksTo(prevPeer);
    connectTo(joiningPeer, shortLink | shortLinkPrev);
    connect(prevPeer, joiningPeer, shortLink | shortLinkSucc);

    joiningPeer->id = requestedId; //we confirm our random id that we have requested
    joiningPeer->updateDisplay(true);

}

// -----------------------------------------------------------------
// LEAVE
// -----------------------------------------------------------------

void Peer::requestLeave(){

    #ifdef DEBUG_LEAVE
           ev << "DEBUG_LEAVE: " << "Request leave" << endl;
    #endif

    /* We atomically destroy the short links, in this way we can have concurrency errors (e.g. another peer needs
     * to connect to the ring).
     * */
    Peer* prevPeer = getPrevNeighbor();
    Peer* nextPeer = getNextNeighbor();
    if (prevPeer == NULL || nextPeer == NULL) throw cRuntimeError("I haven't neighbor!!!");

    disconnectLinksTo(prevPeer);
    disconnectLinksTo(nextPeer);
    connect(prevPeer, nextPeer, shortLink | shortLinkSucc);

    //n - estimation
    double Xs = prevPeer->getSegmentLength() + nextPeer->getSegmentLength() + nextPeer->getNextNeighbor()->getSegmentLength();
    NEstimationMsg* m;
    m = new NEstimationMsg(); m->setN(3/Xs);
    sendDirect(m, nextPeer, "directin");
    m = new NEstimationMsg(); m->setN(3/Xs);
    sendDirect(m, prevPeer, "directin");
    m = new NEstimationMsg(); m->setN(3/Xs);
    sendDirect(m, nextPeer->getNextNeighbor(), "directin");

    //We destroy all long links and we send a message to each peer to say that they need to recreate some long links
    cGate* gate;
    for (int i = 0; i < gateSize("longDistanceLink"); i++) {
        gate = gateHalf("longDistanceLink", cGate::OUTPUT , i);
        if (gate->isConnected()) {
            if (gate->isConnected()) {
                Peer* neighbor = dynamic_cast<Peer*>(gate->getNextGate()->getOwnerModule());

                //Send the message "you need to recreate some long links"
                sendDirect(new cMessage("createLongDistanceLinks"), neighbor, "directin");

                //Disconnect the links (it might be only one)
                disconnectLinksTo(neighbor);
            }
        };
    }

    //We die
    resetPeerState();
    updateDisplay(true);

}

// -----------------------------------------------------------------
// N-Estimation
// -----------------------------------------------------------------
void Peer::manageNUpdate(unsigned int new_n){
    if (state == Connected) {
        //Relinking criterion (see the paper)
        if (((new_n / n) < 0.5) || ((new_n / n) > 2)) {

            //We destroy all long links
            Peer* nextPeer = getNextNeighbor();
            Peer* prevPeer = getPrevNeighbor();
            cGate* gate;
            for (cModule::GateIterator i(this); !i.end(); i++) {
               gate = i();

               // Iterate all *output* links (now we have only long link)
               if (gate->getType() == cGate::OUTPUT) {
                   if (gate->isConnected()) {
                       Peer* neighbor = dynamic_cast<Peer*>(gate->getNextGate()->getOwnerModule());

                       //We must disconnect only the long links
                       if ((neighbor != nextPeer) && (neighbor != prevPeer))
                           disconnectLinksTo(neighbor);
                   }
               }
            }

            //Relinking phase
            state = ReLinking;
            scheduleAt(simTime() + uniform(0,0.01), new cMessage("createLongDistanceLinks"));

            #ifdef DEBUG_RELINKING
                ev << "DEBUG_RELINKING: " << "rapporto superato, si richiede relinking" << endl;
            #endif
        }
    }
}


// -----------------------------------------------------------------
// LOOKUP
// -----------------------------------------------------------------

/* TODO:
 * testare il timeout nel caso arrivi prima il timeout e poi una lookup che ha impiegato moltissimo tempo per far arrivare una risposta
 */
void Peer::requestLookup(double x, lookupCallbackPointer callback, LookupSpecialization ls) {
    assert (!isManagerOf(x));

    unsigned long requestID = ++lookup_requestIDInc;
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
    msg->setSpecialization(ls);
    pair<Peer*,cGate*> nextHop = getNextHopForKey(x);
    if (nextHop.first == NULL){
        assert(knownPeer != NULL);
        sendDirect(msg, knownPeer, "directin");
    }else{
        send(msg, nextHop.second);
    }

    //Timeout
    LookupResponseMsg* msgTimeout = new LookupResponseMsg();
    msgTimeout->setX(x);
    msgTimeout->setManagerID(0);
    msgTimeout->setError(true);
    msgTimeout->setRequestID(requestID);
    msgTimeout->setSpecialization(ls);
    scheduleAt(simTime() + getParentModule()->par("lookupTimeout"), msgTimeout);
}

pair<Peer*,cGate*> Peer::getNextHopForKey(double x) {
    assert (! this->isManagerOf(x));

    Peer* bestPeer = NULL;
    cGate* bestGate = NULL;
    double currBest = -1.0;

    if(getNextNeighbor() != NULL && getNextNeighbor()->isManagerOf(x)){ //If we know that the manager is our next peer
        bestPeer = getNextNeighbor();
        bestGate = gate("shortLink$o", 1);
    }else{

        cGate* gate;
        for (cModule::GateIterator i(this); !i.end(); i++) {
            gate = i();

            // Iterate all *output* links
            if (gate->getType() == cGate::OUTPUT) {
                if (gate->isConnected()) {
                    Peer* neighbor = dynamic_cast<Peer*>(gate->getNextGate()->getOwnerModule());

                    double test;
                    if (getParentModule()->par("unidirectional").boolValue())
                        throw -1; // For now, we don't test unidirectional routing!
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

    }

    return pair<Peer*,cGate*>(bestPeer, bestGate);
}


// -----------------------------------------------------------------
// UTILITY
// -----------------------------------------------------------------

bool Peer::isManagerOf(double x) {
    if (id == -1) return false;

    Peer* previous = getPrevNeighbor();
    assert(previous != NULL);

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
    if (id == -1) return 0;

    Peer* previous = getPrevNeighbor();
    assert(previous != NULL);

    // case 0: THIS == X
    if (previous->id == id) return 0;

    // case 1: |--..--PREVIOUS++THIS--...--|
    if (previous->id < id) return id - previous->id;

    // case 2: |++THIS--...--PREVIOUS++|
    if (id < previous->id) return id + 1.0 - previous->id;

    return 0;
}

Peer* Peer::getPrevNeighbor() {
   if(!gate("shortLink$o", 0)->isConnected()) return NULL;
   return dynamic_cast<Peer*>(gate("shortLink$o", 0)->getNextGate()->getOwnerModule());
}

Peer* Peer::getNextNeighbor() {
   if(!gate("shortLink$o", 1)->isConnected()) return NULL;
   return dynamic_cast<Peer*>(gate("shortLink$o", 1)->getNextGate()->getOwnerModule());
}

// -----------------------------------------------------------------
// GRAPHICS
// -----------------------------------------------------------------

void Peer::updateDisplay(bool displayId = true) {
    if (ev.isGUI()) {
        Peer* prev = getPrevNeighbor();
        if (prev != NULL && displayId) {
            char buf[14];
            snprintf(buf, 14, "]%1.3lf,%1.3lf]", getPrevNeighbor()->id, id);
            getDisplayString().setTagArg("t", 0, buf);
        }

        if (id != -1) {
            int radius = (int)getParentModule()->par("display_radius");
            int centerX = (int)getParentModule()->par("display_center_x");
            int centerY = (int)getParentModule()->par("display_center_y");
            getDisplayString().setTagArg("p", 0, centerX + radius * cos(2*PI * id));
            getDisplayString().setTagArg("p", 1, centerY + radius * sin(2*PI * id));
        } else {
            getDisplayString().setTagArg("p", 0, (int)getParentModule()->par("display_dyn_peer_x"));
            getDisplayString().setTagArg("p", 1, (int)getParentModule()->par("display_dyn_peer_y"));
        }
    }
}

// -----------------------------------------------------------------
// INITIALIZATION
// -----------------------------------------------------------------

void Peer::longDistanceLinksInitialization(){
    int attempts = 0;

    //We must create k long distance links, no more.
    if (getNumberOfConnectedLongLinkGates() >= (int)par("k")) return;

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
        if ((neighbor->getNumberOfConnectedLongLinkGates() < 2 * (int)par("k")) && (!isConnectedTo(neighbor))) { //The number of incoming links per node is bounded by the upper limit of 2k.
            connectTo(neighbor, longDistanceLink);
            scheduleAt(simTime() + uniform(0,0.01), new cMessage("longDistanceLinksInitialization"));
            return;
        } else {
           attempts++;
        }

    }

}

void Peer::initialize() {

    lookup_requestIDInc = 0;
    pendingLookupRequests = new map<unsigned long, PendingLookup>();
    resetPeerState();

    // If I'm static, I don't need to know a "knownPeer" to enter, I'll enter in God's way
    if (par("isStatic").boolValue()) {
        state = Connected;

        //Estimation of n for the STATIC network (remember that, in this case, n is accurate. It's static!)
        n = (int)getParentModule()->par("n_static");
        knownPeer = NULL; //We are always connected, we don't need it.

        //Short Link Creation for the STATIC network
        /* We create it in the ned file, but for completeness this is the code */
        /* Ci servirà quando i nodi dinamici si dovranno connettere */
        /* Peer* nextBrother = dynamic_cast<Peer*>(getParentModule()->getSubmodule(getName(), (getIndex() + 1) % n));
         connectTo(nextBrother, shortLink | shortLinkSucc);
         nextBrother->connectTo(this, shortLink | shortLinkPrev); */

        /* Long Distance Link Creation for the STATIC network
         * We need that all the initialization is done. This can be done with scheduleAt */
        scheduleAt(simTime() + uniform(0,0.01), new cMessage("longDistanceLinksInitialization"));
        //scheduleAt(simTime() + uniform(0,0.01), new cMessage("createLongDistanceLinks"));
    }
    // Otherwise, I'll have to enter through a "knownPeer" chosen uniformly at random from the static peers
    else {
       knownPeer = check_and_cast<Peer*>(getParentModule()->getSubmodule("stat_peer", intrand((int)getParentModule()->par("n_static"))));
      // scheduleAt(simTime() + uniform(1,200), new cMessage("DoJoinMsg")); //DEBUG
    }

    //scheduleAt(simTime() + 12, new cMessage("test"));


    WATCH(state);
    WATCH(n);
    WATCH(lookupFailures);
    WATCH(joinFailuresForElapsedTimeout);

    updateDisplay(false);

}

void Peer::resetPeerState() {
    state = Idle;
    id = (double)par("id");

   // deleteGate("longDistanceLink");
   // addGate("longDistanceLink", cGate::INOUT, true);
    pendingLookupRequests->clear();
    createLongDistanceLinks_rndId = -1;
    createLongDistanceLinks_attempts = -1;
    joinFailuresForElapsedTimeout = 0; //TODO: se questa non deve essere reinizializzata al momento della reincarnazione di un peer togliere da qui
}

// -----------------------------------------------------------------
// MESSAGE MANAGEMENT
// -----------------------------------------------------------------

void Peer::handleMessage(cMessage *msg) {

    if (msg->isName("test")) {

        //TEST LOOKUP
        //if (!isManagerOf(0.5)) requestLookup(0.5, ...);

        //TEST DISCONNECT
        /*for (int v = 0; v < 10; v++) {
            int nn = intrand(n);
            for (cModule::SubmoduleIterator i(getParentModule()); !i.end(); i++) {
               if (!nn) disconnectLinkTo(dynamic_cast<Peer*>(i()));
               nn--;
            }
        }*/

        //if (!(par("isStatic").boolValue())) {
        //    requestLeave();
        //}

        /*NEstimationMsg* m;
        m = new NEstimationMsg(); m->setN(50);
        scheduleAt(simTime(), m);

        delete msg;*/
    }

    else if (msg->isName("DoJoinMsg")) {
        if (state != Idle) throw cRuntimeError("requested a join, but the state is not idle(%d)", state);
        requestJoin();
        #ifdef DEBUG_LEAVE
           // scheduleAt(simTime() + uniform(200,300), new cMessage("DoLeaveMsg")); //DEBUG
        #endif
        delete msg;
    }

    else if (msg->isName("DoLeaveMsg")) {
        if (state == Idle) throw cRuntimeError("requested a leave, but the state is idle");
        requestLeave();
        #ifdef DEBUG_LEAVE
           //scheduleAt(simTime() + uniform(200,300), new cMessage("DoJoinMsg")); //DEBUG
        #endif
        delete msg;
    }

    else if (msg->isName("longDistanceLinksInitialization")) {
        longDistanceLinksInitialization();
        delete msg;
    }

    else if (msg->isName("createLongDistanceLinks")) {
        createLongDistanceLinks();
        delete msg;
    }

    else if (typeid(*msg) == typeid(LookupMsg)) {

        if (state == Idle) {
            delete msg;
        } else {

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

                LookupResponseMsg* rMsg = new LookupResponseMsg();

                if (luMsg->getSpecialization() == lookupJoinSpecialization) {
                    /* It is not only a lookup message, but it requests a join */
                    join(sender, x);
                }

                rMsg->setManagerID(getId());
                rMsg->setX(x);
                rMsg->setRequestID(luMsg->getRequestID());
                rMsg->setHops(luMsg->getHops());
                rMsg->setSpecialization(luMsg->getSpecialization());
                sendDirect(rMsg, sender, "directin");

                delete msg;
            } else {

                #ifdef DEBUG_LOOKUP
                    ev << "DEBUG_LOOKUP: " << "faccio il forward del lookup, requestID: " << luMsg->getRequestID() << endl;
                #endif

                luMsg->setHops(luMsg->getHops() + 1);
                send(luMsg, getNextHopForKey(x).second);
            }

        }

    }

    else if (typeid(*msg) == typeid(LookupResponseMsg)) {

        //if (state != Idle) {

            LookupResponseMsg* mMsg = check_and_cast<LookupResponseMsg*>(msg);
            int requestID = mMsg->getRequestID();

            #ifdef DEBUG_LOOKUP
                if(mMsg->getError()){
                    ev << "DEBUG_LOOKUP: " << "ricevuto timeout di lookup, requestID: " << mMsg->getRequestID() << endl;
                } else {
                    ev << "DEBUG_LOOKUP: " << "ricevuto messaggio di< response del lookup, requestID: " << mMsg->getRequestID() << endl;
                }
            #endif

            map<unsigned long, PendingLookup>::iterator it = pendingLookupRequests->find(requestID);
            if (it != pendingLookupRequests->end()){
                PendingLookup pl = it->second;
                pendingLookupRequests->erase(it);

                #ifdef DEBUG_LOOKUP
                   ev << "DEBUG_LOOKUP: " << "chiamata la funzione di callback per la risposta di lookup, requestID: " << mMsg->getRequestID() << endl;
                #endif

                if (mMsg->getError()) lookupFailures++;

                if(pl.callback != NULL){ //we allow requests without a callback
                    //TODO: Controllare il caso in cui getActiveSimulation()->getModule non fallisca
                    Peer* manager = mMsg->getError() ? NULL : dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(mMsg->getManagerID()));
                    (this->*pl.callback)(manager);
                }
         //   }

        }

        delete msg;

    }

    else if (typeid(*msg) == typeid(NEstimationMsg)) {

        if (state != Idle) {

            #ifdef DEBUG_RELINKING
                ev << "DEBUG_RELINKING: " << "ricevuto messaggio aggiornamento n" << endl;
            #endif

            manageNUpdate((check_and_cast<NEstimationMsg*>(msg))->getN());

        }

        delete msg;
    }

    updateDisplay();

}
