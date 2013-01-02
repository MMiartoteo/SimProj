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

cplusplus {{ 
    #include "peer.h"
    typedef Peer* PeerPtr; 
}}
class noncobject PeerPtr; // announcement for the message compiler

message ManagerMsg {
	PeerPtr manager;  	// The manager ID
	double x;       	// What x the manager has been asked for
	int hops = 0;		// N. of hops take to find manager of x
    
    // Here, you could also add hash-table values... no need for now.
}

message LookupMsg {
	PeerPtr sender; // To be used with sendDirect(...), by the manager of x
                    // to contact the Peer who initiated the lookup routing
	double x;       // Key to be searched for
    int hops = 0;   // Number of hops so far
}