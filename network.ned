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
        double lookupTimeout @unit(s) = 10s;

        int n_static = default(20);
        int n_dynamic = default(10);

        int display_radius = 300;
        int display_center_x = 350;
        int display_center_y = 350;
        @display("bgb=700,700");
        //@display("bgi=maps/europe");
    submodules:
        stat_peer[n_static]: Peer {
            isStatic = true; // The node never leaves, and it is initialized before churn starts
            id = index / n_static; //the id for the STATIC network with n peers
            @display("p=50,50;b=10,10,oval,#777777,,1;t=,,black");
        }
        dyn_peer[n_dynamic]: Peer {
            id = -1;
            @display("p=50,100;b=10,10,oval,#ffffff,,1;t=,,black");
        }
    connections:
        // Short links for the static network
        for i=0..n_static-1 {
            stat_peer[i].shortLink[1] <--> ShortLinkChannel <--> stat_peer[(i+1)%n_static].shortLink[0];
        }
}
