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

#ifndef LOOKUPMSG_H_
#define LOOKUPMSG_H_

#include "peer.h"
#include <cmessage.h>

class LookupMsg: public cMessage {
    public:
        Peer* sender;    // To be used with sendDirect(...), by the manager of x
                        // to contact the Peer who initiated the lookup routing
        double x;       // Key to be searched for
        int hops;       // Number of hops so far

        LookupMsg(Peer* sender, double x);
        virtual ~LookupMsg();
};

#endif /* LOOKUPMSG_H_ */
