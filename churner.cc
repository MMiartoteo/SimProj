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
    for (cModule::SubmoduleIterator iter(getParentModule()->getSubmodule("dyn_peer")); !iter.end(); iter++) {
        /* SubmoduleIterator � un iteratore... non c'entra niente con il foreach
         * non voglio nemmeno sapere cos'è un iteratore per C++! L'ho rimosso e ne vado fiero */
        outPeers.push_back(check_and_cast<Peer*>(iter()));
    }
}


void Churner::handleMessage(cMessage *msg) {

    if (msg->isName("doOneJoin")) {
        Peer* peer = outPeers[intrand((int)outPeers.size())];
        inPeers.push_back(peer);

        /* speri veramente che il metodo remove (che non esiste) ti cerchi un elemento nella lista che sia uguale a peer
         * secondo un metodo di comparazione pi� o meno preciso, e se ci sono dei duplicati?
         * nelle liste c'è! :D
         *
         * A me sembra che sto vettore lo stai usando come una map...
         * a boh! cmq, penso che l'idea l'avrai capita.
         * */
        //outPeers.remove(peer);

        cMessage* msg = new cMessage(); //DoJoinMsg* msg = new DoJoinMsg(); //DoJoinMsg?
        sendDirect(msg, peer, "directin");

        scheduleAt(simTime() + (int)getParentModule()->par("join_freq"), new cMessage("doOneJoin"));
    }

    else if (msg->isName("doOneLeave")) {
        Peer* peer = inPeers[intrand((int)outPeers.size())];
        //inPeers.remove(peer);
        outPeers.push_back(peer);

        cMessage* msg = new cMessage(); //DoLeaveMsg* msg = new DoLeaveMsg(); //DoLeaveMsg?
        sendDirect(msg, peer, "directin");

        scheduleAt(simTime() + (int)getParentModule()->par("leave_freq"), new cMessage("doOneLeave"));
    }

    delete msg;
}
