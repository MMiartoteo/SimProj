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

#include "churner.h"
#include "peer.h"
#include <string>
#include <sstream>
#include <cassert>
#include <cmath>
#include <assert.h>

using namespace std;

Define_Module(Churner);


void Churner::initialize() {
    /* At the beginning, every dynamic peer is out of the network. */
    for (int i=0; i<=(int)getParentModule()->par("n_dynamic")-1; ++i) {
        Peer* peer = check_and_cast<Peer*>(getParentModule()->getSubmodule("dyn_peer", i));
        assert(peer->state == Peer::Idle);
        outPeers.push_back(peer->getId());
    }

    test_type = getParentModule()->par("test").stringValue();

    //scheduleAt(simTime() + par("join_freq").doubleValue(), new cMessage("doOneJoin"));
    //scheduleAt(simTime() + par("leave_freq").doubleValue(), new cMessage("doOneLeave"));

    scheduleAt(simTime() + 20.0, new cMessage("start"));

    N_L = N_S = (int)getParentModule()->par("n_static"); //TODO: Sto N serve?
    N_of_joins = 0;
    N_of_leaves = 0;

    join_active = true;
    leave_active = true;

    ///inGoingSizeSignal = registerSignal("inGoingSizeSig");
    NSSignal = registerSignal("NSSig");
}

int Churner::getN_L() {
    Enter_Method("getN_L()");
    return N_L;
}

int Churner::getN_S() {
    Enter_Method("getN_S()");
    return N_S;
}

//void Churner::incrementN() {  //TODO: serve? MI PARE DI NO
    // Chiamata da un peer che ha completato la join
//    N++;
//}

//void Churner::decrementN() { //TODO: serve? MI PARE DI NO
    // Chiamata da un peer che ha completato la leave
//    N--;
//}

void Churner::scheduleJoin() {
    //if (! (test_type == "join" && N_of_joins >= (int)par("noOfJoins"))) { //TODO: ???

    //TODO: configurabile per esempio per un test general questo if fallisce sempre la simulazione termina subito
    if (N_of_joins + inGoing.size() < (int)par("noOfJoins")) {
        //cout << "schedule join" << endl;
        // Il comportamento normale è quello di schedulare join continuamente
        // Ma non lo facciamo se il test è di "join" e abbiamo raggiunto il max n. di join nel test
        // In questo modo il Churner morirà e la simulazione avrà fine automaticamente
        if (outPeers.size() == 0) {
            //scheduleAt(simTime() + 10.0, new cMessage("doOneJoin"));
            join_active = false;
        } else {
            scheduleAt(simTime() + par("join_freq"), new cMessage("doOneJoin"));
        }
    }
}

void Churner::scheduleLeave() {
    //if (! (test_type == "join" && N_of_joins >= (int)par("noOfJoins"))) { //TODO: ???

    //TODO: configurabile per esempio per un test general questo if fallisce sempre la simulazione termina subito
    if (N_of_leaves + outGoing.size() < (int)par("noOfLeaves")) {
        //cout << "schedule leave" << endl;
        // Il comportamento normale è quello di schedulare leave continuamente
        // Ma non lo facciamo se il test è di "join" e abbiamo raggiunto il max n. di join nel test
        // In questo modo il Churner morirà e la simulazione avrà fine automaticamente
        if (inPeers.size() == 0) {
            //scheduleAt(simTime() + 10.0, new cMessage("doOneLeave"));
            leave_active = false;
        } else {
            scheduleAt(simTime() + par("leave_freq"), new cMessage("doOneLeave"));
        }
    }
}

void Churner::incrN_S() {
    N_S++;
}

/**
 * This updates inPeers, which is the list of peers that can be kicked out of the network.
 * We can kick out (i.e. force a "leave") only peers who have finished joining the network
 */
void Churner::setPeerIn(int peer_idx) {
    Enter_Method("setPeerIn(int peer_idx)");
    for (vector<int>::iterator p = inGoing.begin() ; p != inGoing.end(); ++p) {
        if (*p == peer_idx) {
            inPeers.push_back(peer_idx);

            //cout << "join completion detected " << dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(peer_idx))  << endl;
            ev << "CHURNER: join completion detected " << dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(peer_idx)) << endl;

            N_of_joins++;
            N_L++;

            inGoing.erase(p);
            break;
        }
    }
    if (! leave_active) {
        leave_active = true;
        scheduleLeave();
    }
    emit(NSSignal, N_S);
}

/**
 * This updates outPeers, which is the list of peers that can be inserted in the network.
 * We can insert (i.e. make a "join") peers who have finished leaving the network,
 * therefore only Idle peers are candidates.
 */
void Churner::setPeerOut(int peer_idx) {
    Enter_Method("setPeerOut(int peer_idx)");
    for (vector<int>::iterator p = outGoing.begin() ; p != outGoing.end(); ++p) {
       if (*p == peer_idx) {
           outPeers.push_back(peer_idx);

           //cout << "leave completion detected " << dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(peer_idx)) << endl;
           ev << "CHURNER: leave completion detected " << dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(peer_idx)) << endl;

           N_of_leaves++;
           N_L--;
           N_S--;

           outGoing.erase(p);
           break;
       }
    }
    if (! join_active) {
        join_active = true;
        scheduleJoin();
    }
    emit(NSSignal, N_S);
}

void Churner::handleMessage(cMessage *msg) {
    assert(N_L <= N_S);

    //emit(inGoingSizeSignal, inGoing.size());

    assert(inGoing.size() + outGoing.size() + inPeers.size() + outPeers.size() == (unsigned int)getParentModule()->par("n_dynamic"));

    //cout << outPeers.size() << " " << inGoing.size() << " " << inPeers.size() << " " << outGoing.size() << endl;

    //TODO: ???
    //for (vector<Peer*>::iterator p = inPeers.begin() ; p != inPeers.end(); ) {
    //        cout << *p << endl;
    //    }

    if (msg->isName("start")) {
        scheduleJoin();
        scheduleLeave();
    }

    else if (msg->isName("doOneJoin")) {

        if (outPeers.size() > 0) { // <-- Modificare qui se si vuole fare altro
            // Select a peer randomly from the ones "outside" of the network
            //Peer* peer = outPeers[intrand((int)outPeers.size())]; //TODO: ???
            int peer_idx = outPeers[0];
            Peer* peer = dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(peer_idx));
            //cout << "churner ask join to " << peer << endl;

            // Erase peer from outPeer list
            bool found = false;
            for (vector<int>::iterator p = outPeers.begin() ; p != outPeers.end(); ++p) {
                if (*p == peer_idx) {
                    outPeers.erase(p);
                    found = true;
                    break;
                }
            }
            assert(found);

            // Add it to the inGoing (neither "in" nor "out" of the network)
            inGoing.push_back(peer_idx);

            //cout << "join told to peer " << peer << endl;

            // Tell the peer to join the network
            cMessage* msg = new cMessage("DoJoinMsg");
            sendDirect(msg, peer, "directin");
            ev << "CHURNER: richiesta di join per il peer" << peer << endl;
            //if (peer->state != Peer::Idle) throw cRuntimeError("requested a join, but the state is not idle(%d)", peer->state);
            //peer->requestJoin(-1); //TODO: ???
        }

        scheduleJoin();
    }

    else if (msg->isName("doOneLeave")) {

        if (inPeers.size() > 0) { // <-- Modificare qui se si vuole fare altro
            // Select a peer randomly from the ones "inside" of the network
            //Peer* peer = inPeers[intrand((int)inPeers.size())]; //TODO: ???
            int peer_idx = inPeers[0];
            Peer* peer = dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(peer_idx));

            // Remove peer from inPeer list
            bool found = false;
            for (vector<int>::iterator p = inPeers.begin() ; p != inPeers.end(); ++p) {
                if (*p == peer_idx) {
                    inPeers.erase(p);
                    found = true;
                    break;
                }
            }
            assert(found);

            // Add it to the outGoing (neither "in" nor "out" of the network)
            outGoing.push_back(peer_idx);

            //cout << "leave told to peer " << peer << endl;

            // Tell the peer to leave the network
            cMessage* msg = new cMessage("DoLeaveMsg");
            sendDirect(msg, peer, "directin");
            ev << "CHURNER: richiesta di leave per il peer" << peer << endl;
            //peer->requestLeave(); //TODO: ???
        }

        scheduleLeave();
    }

    delete msg;
}
