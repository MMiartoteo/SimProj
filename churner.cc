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
        outPeers.push_back(peer);
    }

    scheduleAt(simTime() + (int)par("join_freq"), new cMessage("doOneJoin"));
    //scheduleAt(simTime() + (int)par("leave_freq"), new cMessage("doOneLeave"));
}


void Churner::handleMessage(cMessage *msg) {

    if (msg->isName("doOneJoin")) {
        if (outPeers.size() <= 0) return; // <-- Modificare qui se si vuole fare altro

        Peer* peer = outPeers[intrand((int)outPeers.size())];
        //cout << peer << endl;

        inPeers.push_back(peer);

        for (vector<Peer*>::iterator p = outPeers.begin() ; p != outPeers.end(); ++p) {
            if (*p == peer) {
                outPeers.erase(p);
                break;
            }
        }

        cMessage* msg = new cMessage("DoJoinMsg"); //DoJoinMsg* msg = new DoJoinMsg(); //DoJoinMsg?
        sendDirect(msg, peer, "directin");

        scheduleAt(simTime() + (int)par("join_freq"), new cMessage("doOneJoin"));
    }

    else if (msg->isName("doOneLeave")) {
        if (inPeers.size() <= 0) return; // <-- Modificare qui se si vuole fare altro

        Peer* peer = inPeers[intrand((int)inPeers.size())];

        outPeers.push_back(peer);

        for (vector<Peer*>::iterator p = inPeers.begin() ; p != inPeers.end(); ++p) {
            if (*p == peer) {
                inPeers.erase(p);
                break;
            }
        }

        cMessage* msg = new cMessage("DoLeaveMsg"); //DoLeaveMsg* msg = new DoLeaveMsg(); //DoLeaveMsg?
        sendDirect(msg, peer, "directin");

        scheduleAt(simTime() + (int)par("leave_freq"), new cMessage("doOneLeave"));
    }

    delete msg;
}
