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
		int numPeers = default(20);
	types:
	    channel LinkChannel extends ned.DelayChannel {
	        delay = 100ms;
	    }
	    channel ShortLinkChannel extends LinkChannel {
		    @display("ls=#FF0F18");
		}
		channel LongDistanceLinkChannel extends LinkChannel {
		    @display("ls=#0917FF");
		}
    submodules:
        peer[numPeers]: Peer {
            @display("p=50,50,ring,200,200;b=20,20,oval,#FF0000,,1");
        }
        
    // Generiamo tutto dinamicamente
    //connections:
    
        //for i=0..numPeers-1 {
        //    peer[i].shortLinkOut++ --> ShortLinkChannel --> peer[(i+1)%numPeers].shortLinkIn++;
			//peer[i].shortLinkIn++ <-- ShortLinkChannel <-- peer[(i+1)%numPeers].shortLinkOut++;
        //}

        //Genera dei collegamenti multipli, ed anche per altri motivi questi collegamenti vanno
        //Generati via codice
        //for i=0..numPeers-1 {
        //    peer[i].longDistanceLink++ <--> LongDistanceLinkChannel <--> peer[(i+uniform(1,numPeers-1))%numPeers].longDistanceLink++;
        //}



}
