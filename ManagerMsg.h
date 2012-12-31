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

#ifndef MANAGERMSG_H_
#define MANAGERMSG_H_

#include "peer.h"
#include <cmessage.h>

class ManagerMsg: public cMessage {
    public:

        Peer* manager;  // The manager
        double x;       // What x the manager has been asked for
        int hops;       // N. of hops take to find manager of x
        // Here, you could also add hash-table values... no need for now.

        ManagerMsg(Peer* manager, double x);
        virtual ~ManagerMsg();
};

#endif /* MANAGERMSG_H_ */
