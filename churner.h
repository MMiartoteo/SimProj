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

//#define DEBUG_LOOKUP
//#define DEBUG_JOIN
//#define DEBUG_CREATELONGLINK

#ifndef __SYMPHONY_CHURNER_H_
#define __SYMPHONY_CHURNER_H_

#include "peer.h"
#include <omnetpp.h>
#include <utility>
#include <vector>
using namespace std;

class Churner : public cSimpleModule {

public:
    int getN_S(); // Read N_S
    int getN_L(); // Read N_L
    void incrementN(); // Called by peers that join and leave the network
    void decrementN(); // Called by peers that join and leave the network

    void setPeerIn(int peer_idx);
    void setPeerOut(int peer_idx);

    void incrN_S();

protected:

    unsigned int N_S;   // Number of peers that have established all their Short links
    unsigned int N_L;   // Number of peer that have established all their Long links (N_S <= N_L)

    unsigned int N_of_joins;
    unsigned int N_of_leaves;

    bool join_active;
    bool leave_active;

    string test_type;

    vector<int> inPeers;  // peers who are in the network and that we can kick out (Connected or ReLinking)
    vector<int> outPeers; // peers who are outside of the network, that we can make join it (Idle)
    vector<int> inGoing;  // peers who are still doing either a join or a leave -- still pending, nor "in" or "out"
    vector<int> outGoing; // peers who are still doing either a join or a leave -- still pending, nor "in" or "out"

    //simsignal_t inGoingSizeSignal;
    simsignal_t NSSignal;

    virtual void scheduleJoin();
    virtual void scheduleLeave();

   // -----------------------------------------------------------------
   // INITIALIZATION
   // -----------------------------------------------------------------

    virtual void initialize();

   // -----------------------------------------------------------------
   // MESSAGE MANAGEMENT
   // -----------------------------------------------------------------

    virtual void handleMessage(cMessage *msg);

};

#endif
