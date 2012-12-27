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
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

        /***
         * Connect the peer with the index peerIndexFrom to the peer with the index peerIndexTo
         *
         * @param peerIndexFrom the index of the peer you want to connect.
         * @param peerIndexTo the index of the peer you want to connect to.
         *
         * @return true if the connection has (and can) been established.
         * */
        virtual bool connectLongDistanceLink(int peerIndexFrom, int peerIndexTo);

        /***
         * Connect the current peer to the peer with the index peerIndexTo
         * @param peerIndexFrom the index of the peer you want to connect.
         *
         * @return true if the connection has (and can) been established.
         * */
        virtual bool connectLongDistanceLinkTo(int peerIndexTo);

        /***
         * Connect the peer with the index peerIndexFrom to the current peer
         *
         * @param peerIndexFrom the index of the peer you want to connect.
         *
         * @return true if the connection has (and can) been established.
         * */
        virtual bool connectLongDistanceLinkFrom(int peerIndexFrom);

        /***
         * Check if the peer with the index peerIndexFrom is is connected to another peer (with the index peerIndexTo)
         * through a short link or through a long distance link
         */
        virtual bool areConnected(int peerIndexFrom, int peerIndexTo);

        /***
         * Check if the current peer is connected to another peer (with the index peerIndexTo)
         * through a short link or through a long distance link
         */
        virtual bool isConnectedTo(int peerIndexTo);

        /***
         * Check if the peer (with the index peerIndexFrom) is connected to the current peer
         * through a short link or through a long distance link
         */
        virtual bool isConnectedFrom(int peerIndexFrom);


};

#endif
