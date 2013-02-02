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

network Network
{
    parameters:
        //Bidiretional treat the incoming long distance link as additional links
        //In the unidirectional, if we have a long distance link L that connect A to B, we
        //have that L$i in A is not used and L$o in B is not used. This is due to the unidirectionality.   
        bool unidirectional = default(false); 
        
        int n = default(20);
    submodules:
        peer[n]: Peer {
            isMemberOfAStaticNetwork = true;
            id = index / n; //the id for the STATIC network with n peers
            @display("p=-50,50,ring,-350,350;b=20,20,oval,#777777,,1;t=,,black");
        }
    connections:
        for i=0..n-1 {
    		peer[i].shortLink[1] <--> ShortLinkChannel <--> peer[(i+1)%n].shortLink[0];
        }
}
