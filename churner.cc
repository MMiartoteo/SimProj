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
    scheduleJoin();
    scheduleLeave();

    N = (int)getParentModule()->par("n_static");
    N_of_joins = 0;
    N_of_leaves = 0;
}

int Churner::getN() {
    return N;
}

void Churner::incrementN() {
    // Chiamata da un peer che ha completato la join
    N++;
}

void Churner::decrementN() {
    // Chiamata da un peer che ha completato la leave
    N--;
}

void Churner::scheduleJoin() {
    //if (! (test_type == "join" && N_of_joins >= (int)par("noOfJoins"))) {
    if (N_of_joins + inGoing.size() < (int)par("noOfJoins")) {
        //cout << "schedule join" << endl;
        // Il comportamento normale è quello di schedulare join continuamente
        // Ma non lo facciamo se il test è di "join" e abbiamo raggiunto il max n. di join nel test
        // In questo modo il Churner morirà e la simulazione avrà fine automaticamente
        scheduleAt(simTime() + par("join_freq"), new cMessage("doOneJoin"));
    }
}

void Churner::scheduleLeave() {
    //if (! (test_type == "join" && N_of_joins >= (int)par("noOfJoins"))) {
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
 * We can kick out (i.e. force a "leave") only peers who have finished joining the network,
 * therefore Connected and ReLinking peers are both candidates.
 * Notice, a peer has finished joining only if its long-distance links have been created.
 */
void Churner::update_inPeers() {
    for (vector<Peer*>::iterator p = inGoing.begin() ; p != inGoing.end(); ) {
        if ((*p)->state == Peer::Connected || (*p)->state == Peer::ReLinking) {
            inPeers.push_back(*p);
            p = inGoing.erase(p);

            N_of_joins++;
            cout << "join completion detected " << *p  << endl;

            N++;
        }
        else {
            p++;
        }
    }
    //cout << inPeers.size() + (int)getParentModule()->par("n_static") << " " << N << endl;
    //assert(inPeers.size() + (int)getParentModule()->par("n_static") == N || inPeers.size() + (int)getParentModule()->par("n_static")+1 == N);
}

/**
 * This updates outPeers, which is the list of peers that can be inserted in the network.
 * We can insert (i.e. make a "join") peers who have finished leaving the network,
 * therefore only Idle peers are candidates.
 * Notice, after a peer has finished leaving its state returns to Idle.
 */
void Churner::update_outPeers() {
    for (vector<Peer*>::iterator p = outGoing.begin() ; p != outGoing.end(); ) {
        if ((*p)->state == Peer::Idle) {
            outPeers.push_back(*p);
            p = outGoing.erase(p);

            N_of_leaves++;
            cout << "leave completion detected " << *p << endl;

            N--;
        }
        else {
            p++;
        }

    }
}


void Churner::handleMessage(cMessage *msg) {

    assert(inGoing.size() + outGoing.size() + inPeers.size() + outPeers.size() == (int)getParentModule()->par("n_dynamic"));

    cout << outPeers.size() << " " << inGoing.size() << " " << inPeers.size() << " " << outGoing.size() << endl;

    //for (vector<Peer*>::iterator p = inPeers.begin() ; p != inPeers.end(); ) {
    //        cout << *p << endl;
    //    }

    if (msg->isName("doOneJoin")) {
        update_outPeers();

        if (outPeers.size() > 0) { // <-- Modificare qui se si vuole fare altro
            // Select a peer randomly from the ones "outside" of the network
            //Peer* peer = outPeers[intrand((int)outPeers.size())];
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

            // Add it to the purgatory (neither "in" nor "out" of the network)
            inGoing.push_back(peer);

            cout << "join told to peer " << peer << endl;

            // Tell the peer to join the network
            cMessage* msg = new cMessage("DoJoinMsg");
            sendDirect(msg, peer, "directin");
            ev << "CHURNER: richiesta di join per il peer" << peer << endl;
            if (peer->state != Peer::Idle) throw cRuntimeError("requested a join, but the state is not idle(%d)", peer->state);
            //peer->requestJoin(-1);
        }

        scheduleJoin();
    }

    else if (msg->isName("doOneLeave")) {
        update_inPeers();

        if (inPeers.size() > 0) { // <-- Modificare qui se si vuole fare altro
            // Select a peer randomly from the ones "inside" of the network
            //Peer* peer = inPeers[intrand((int)inPeers.size())];
            Peer* peer = outPeers[0];

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

            // Add it to the purgatory (neither "in" nor "out" of the network)
            outGoing.push_back(peer);

            cout << "leave told to peer " << peer << endl;

            // Tell the peer to leave the network
            cMessage* msg = new cMessage("DoLeaveMsg"); //DoLeaveMsg* msg = new DoLeaveMsg(); //DoLeaveMsg?
            sendDirect(msg, peer, "directin");
            ev << "CHURNER: richiesta di leave per il peer" << peer << endl;
            //peer->requestLeave();
        }

        scheduleLeave();
    }

    delete msg;
}
