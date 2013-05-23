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
        outPeers.push_back(peer);
    }

    test_type = getParentModule()->par("test").stringValue();

    //scheduleAt(simTime() + par("join_freq").doubleValue(), new cMessage("doOneJoin"));
    //scheduleAt(simTime() + par("leave_freq").doubleValue(), new cMessage("doOneLeave"));

    scheduleAt(simTime() + 20.0, new cMessage("start"));

    N = (int)getParentModule()->par("n_static"); //TODO: Sto N serve?
    N_of_joins = 0;
    N_of_leaves = 0;
}

int Churner::getN() {
    return N;
}

void Churner::incrementN() {  //TODO: serve?
    // Chiamata da un peer che ha completato la join
    N++;
}

void Churner::decrementN() { //TODO: serve?
    // Chiamata da un peer che ha completato la leave
    N--;
}

void Churner::scheduleJoin() {
    //if (! (test_type == "join" && N_of_joins >= (int)par("noOfJoins"))) { //TODO: ???

    //TODO: configurabile per esempio per un test general questo if fallisce sempre la simulazione termina subito
    if (N_of_joins + inGoing.size() < (int)par("noOfJoins")) {
        //cout << "schedule join" << endl;
        // Il comportamento normale è quello di schedulare join continuamente
        // Ma non lo facciamo se il test è di "join" e abbiamo raggiunto il max n. di join nel test
        // In questo modo il Churner morirà e la simulazione avrà fine automaticamente
        scheduleAt(simTime() + par("join_freq"), new cMessage("doOneJoin"));
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
        scheduleAt(simTime() + par("leave_freq"), new cMessage("doOneLeave"));
    }
}

/**
 * This updates inPeers, which is the list of peers that can be kicked out of the network.
 * We can kick out (i.e. force a "leave") only peers who have finished joining the network
 */
void Churner::setPeerIn(Peer* peer) {
    for (vector<Peer*>::iterator p = inGoing.begin() ; p != inGoing.end(); ++p) {
        if (*p == peer) {
            inPeers.push_back(peer);

            cout << "join completion detected " << peer  << endl;
            ev << "CHURNER: join completion detected " << peer << endl;

            N_of_joins++;
            N++;

            inGoing.erase(p);
            break;
        }
    }
}

/**
 * This updates outPeers, which is the list of peers that can be inserted in the network.
 * We can insert (i.e. make a "join") peers who have finished leaving the network,
 * therefore only Idle peers are candidates.
 */
void Churner::setPeerOut(Peer* peer) {
    for (vector<Peer*>::iterator p = outGoing.begin() ; p != outGoing.end(); ++p) {
       if (*p == peer) {
           outPeers.push_back(peer);

           cout << "leave completion detected " << peer  << endl;
           ev << "CHURNER: leave completion detected " << peer << endl;

           N_of_leaves++;
           N--;

           outGoing.erase(p);
           break;
       }
   }
}

void Churner::handleMessage(cMessage *msg) {

    assert(inGoing.size() + outGoing.size() + inPeers.size() + outPeers.size() == (unsigned int)getParentModule()->par("n_dynamic"));

    cout << outPeers.size() << " " << inGoing.size() << " " << inPeers.size() << " " << outGoing.size() << endl;

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
            Peer* peer = outPeers[0];
            //cout << "churner ask join to " << peer << endl;

            // Erase peer from outPeer list
            bool found = false;
            for (vector<Peer*>::iterator p = outPeers.begin() ; p != outPeers.end(); ++p) {
                if (*p == peer) {
                    outPeers.erase(p);
                    found = true;
                    break;
                }
            }
            assert(found);

            // Add it to the inGoing (neither "in" nor "out" of the network)
            inGoing.push_back(peer);

            cout << "join told to peer " << peer << endl;

            // Tell the peer to join the network
            cMessage* msg = new cMessage("DoJoinMsg");
            sendDirect(msg, peer, "directin");
            ev << "CHURNER: richiesta di join per il peer" << peer << endl;
            if (peer->state != Peer::Idle) throw cRuntimeError("requested a join, but the state is not idle(%d)", peer->state);
            //peer->requestJoin(-1); //TODO: ???
        }

        scheduleJoin();
    }

    else if (msg->isName("doOneLeave")) {

        if (inPeers.size() > 0) { // <-- Modificare qui se si vuole fare altro
            // Select a peer randomly from the ones "inside" of the network
            //Peer* peer = inPeers[intrand((int)inPeers.size())]; //TODO: ???
            Peer* peer = inPeers[0];

            // Remove peer from inPeer list
            bool found = false;
            for (vector<Peer*>::iterator p = inPeers.begin() ; p != inPeers.end(); ++p) {
                if (*p == peer) {
                    inPeers.erase(p);
                    found = true;
                    break;
                }
            }
            assert(found);

            // Add it to the outGoing (neither "in" nor "out" of the network)
            outGoing.push_back(peer);

            cout << "leave told to peer " << peer << endl;

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
