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

    scheduleAt(simTime() + (int)par("join_freq"), new cMessage("doOneJoin"));
    scheduleAt(simTime() + (int)par("leave_freq"), new cMessage("doOneLeave"));
}

/**
 * This updates inPeers, which is the list of peers that can be kicked out of the network.
 * We can kick out (i.e. force a "leave") only peers who have finished joining the network,
 * therefore Connected and ReLinking peers are both candidates.
 * Notice, a peer has finished joining only if its long-distance links have been created.
 */
void Churner::update_inPeers() {
    for (vector<Peer*>::iterator p = purgatory.begin() ; p != purgatory.end(); ++p) {
        cout << 'P ' << *p << endl;
        if ((*p)->state == Peer::Connected || (*p)->state == Peer::ReLinking) {
            inPeers.push_back(*p);
        }
    }
}

/**
 * This updates outPeers, which is the list of peers that can be inserted in the network.
 * We can insert (i.e. make a "join") peers who have finished leaving the network,
 * therefore only Idle peers are candidates.
 * Notice, after a peer has finished leaving its state returns to Idle.
 */
void Churner::update_outPeers() {
    for (vector<Peer*>::iterator p = purgatory.begin() ; p != purgatory.end(); ++p) {
        if ((*p)->state == Peer::Idle) {
            outPeers.push_back(*p);
        }
    }
}


void Churner::handleMessage(cMessage *msg) {

    if (msg->isName("doOneJoin")) {
        update_outPeers();

        if (outPeers.size() > 0) { // <-- Modificare qui se si vuole fare altro
            // Select a peer randomly from the ones "outside" of the network
            Peer* peer = outPeers[intrand((int)outPeers.size())];
            cout << "churner ask join to " << peer << endl;


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
            purgatory.push_back(peer);

            // Tell the peer to join the network
            cMessage* msg = new cMessage("DoJoinMsg");
            sendDirect(msg, peer, "directin");
        }

        scheduleAt(simTime() + (int)par("join_freq"), new cMessage("doOneJoin"));
    }

    else if (msg->isName("doOneLeave")) {
        cout << "before " << endl;
        cout << inPeers.size() << endl;
        update_inPeers();
        cout << "after " << endl;
        cout << inPeers.size() << endl;

        if (inPeers.size() > 0) { // <-- Modificare qui se si vuole fare altro
            // Select a peer randomly from the ones "inside" of the network
            Peer* peer = inPeers[intrand((int)inPeers.size())];
            cout << peer << endl;

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
            purgatory.push_back(peer);

            // Tell the peer to leave the network
            cMessage* msg = new cMessage("DoLeaveMsg"); //DoLeaveMsg* msg = new DoLeaveMsg(); //DoLeaveMsg?
            sendDirect(msg, peer, "directin");
        }

        scheduleAt(simTime() + (int)par("leave_freq"), new cMessage("doOneLeave"));
    }

    delete msg;
}
