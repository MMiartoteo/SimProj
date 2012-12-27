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

package symphony;

//
// TODO auto-generated type
//
network StaticNetwork
{
	parameters:
		int numNodes = default(20);
	types:
	    channel ShortLinkChannel extends ned.DelayChannel {
		    @display("ls=#FF0F18");
	    	delay = 1s;
		}
		channel LongDistanceLinkChannel extends ned.DelayChannel {
		    @display("ls=#0917FF");
	    	delay = 1s;
		}
    submodules:
        peer[numNodes]: Peer {
            @display("p=50,50,ring,200,200;b=20,20,oval,#AAAAAA,,1");
        }
    connections:
        for i=0..numNodes-1 {
            peer[i].shortLinkOut++ --> ShortLinkChannel --> peer[(i+1)%numNodes].shortLinkIn++;
			peer[i].shortLinkIn++ <-- ShortLinkChannel <-- peer[(i+1)%numNodes].shortLinkOut++;
        }

        //Genera dei collegamenti multipli, ed anche per altri motivi questi collegamenti vanno
        //Generati via codice
        //for i=0..numNodes-1 {
        //    peer[i].longDistanceLink++ <--> LongDistanceLinkChannel <--> peer[(i+uniform(1,numNodes-1))%numNodes].longDistanceLink++;
        //}



}
