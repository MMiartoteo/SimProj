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

#ifndef __SYMPHONY_PEER_H_
#define __SYMPHONY_PEER_H_

#include <omnetpp.h>

class Peer : public cSimpleModule {

    enum LinkType {shortLink, longDistanceLink};

    protected:

        int n; //Number of peers in the network, it can be an extimation
        double id; //Own id. For the STATIC network the id can be found in the parameters

        /* OMNET methods */
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

        /***
         * Connect the peer with the id peerIdFrom to the peer with the id peerIdTo
         *
         * @return true if the connection has (and can) been established.
         * */
        virtual bool connect(int peerIdFrom, int peerIdTo, LinkType linkType);

        /***
         * Connect the current peer to the peer with the id peerIdTo
         *
         * @return true if the connection has (and can) been established.
         * */
        virtual bool connectTo(int peerIdTo, LinkType linkType);

        /***
         * Connect the peer with the id peerIdFrom to the current peer
         *
         * @return true if the connection has (and can) been established.
         * */
        virtual bool connectFrom(int peerIdFrom, LinkType linkType);

        /***
         * Disconnect the link from the peer pFrom (with the id peerIdFrom) to the peer pTo (with the id peerIdTo)
         *
         * @return true if there is a connection between the two peers and it has been eliminated.
         * */
        virtual bool disconnect(int peerIdFrom, int peerIdTo, LinkType linkType);

        /***
         * Disconnect the link from the current peer to the peer pTo (with the id peerIdTo)
         *
         * @return true if there is a connection between the two peers and it has been eliminated.
         * */
        virtual bool disconnectLinkTo(int peerIdTo, LinkType linkType);

        /***
         * Disconnect the link from the peer pFrom (with the id peerIdFrom) to the current peer
         *
         * @return true if there is a connection between the two peers and it has been eliminated.
         * */
        virtual bool disconnectLinkFrom(int peerIdFrom, LinkType linkType);

        /***
         * Check if the peer with the id peerIdFrom is is connected to another peer (with the id peerIdTo)
         * through a short link or through a long distance link
         */
        virtual bool areConnected(int peerIdFrom, int peerIdTo);

        /***
         * Check if the current peer is connected to another peer (with the id peerIdTo)
         * through a short link or through a long distance link
         */
        virtual bool isConnectedTo(int peerIdTo);

        /***
         * Check if the peer (with the id peerIdFrom) is connected to the current peer
         * through a short link or through a long distance link
         */
        virtual bool isConnectedFrom(int peerIdFrom);

        /**
         * Update the display (text, position, etc.) of the peer on the canvas using the variables of the peer (id, status, etc.)
         * */
        virtual void updateDisplay();

        /**
         * If the peer is a member of the static network, this is the method that initialize it
         * */
        virtual void peerInizializationForStaticNetwork();

};

#endif
