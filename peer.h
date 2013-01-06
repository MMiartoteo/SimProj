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
#include <utility>
using namespace std;

class Peer : public cSimpleModule {

    /**
     * The type of the link
     */
    enum LinkType {shortLink = 0x0, longDistanceLink = 0x2};

    /**
     * The type of the short link: it can be the link to the previous neighbor or to the successor
     */
    enum ShortLinkType {shortLinkPrev = 0x0, shortLinkSucc = 0x1};

    enum CallbackType {join, longLinkCreation, query};

    typedef struct {
        int requestID;
        double key;
        CallbackType callback; //determines the callback
    } PendingLookup;

    protected:

        int n; //Number of peers in the network, it can be an extimation
        double id; //Own id. For the STATIC network the id can be found in the parameters

        list<PendingLookup>* pendingLookupRequests;
        int lookup_requestIDInc;

        /* OMNET methods */
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

        /***
         * Connect the peer pFrom to the peer pTo
         *
         * to specify that the link is a short link, and it is the link to the previous neighbor
         * you can write shortLink | shortLinkPrev
         * ...
         *
         * @return true if the connection has (and can) been established.
         * */
        virtual bool connect(Peer* pFrom, Peer* pTo, long linkType);

        /***
         * Connect the current peer to the peer pTo
         *
         * @return true if the connection has (and can) been established.
         * */
        virtual bool connectTo(Peer* pTo, long linkType);

        /***
         * Disconnect the link from the peer pFrom to the peer pTo
         *
         * @return true if there is a connection between the two peers and it has been eliminated.
         * */
        virtual bool disconnect(Peer* pFrom, Peer* pTo);

        /***
         * Disconnect the link from the current peer to the peer pTo
         *
         * @return true if there is a connection between the two peers and it has been eliminated.
         * */
        virtual bool disconnectLinkTo(Peer* pTo);

        /***
         * Check if the peer pFrom is is connected to another peer pTo
         * through a short link or through a long distance link
         */
        virtual bool areConnected(Peer* pFrom, Peer* pTo);

        /***
         * Check if the current peer is connected to another peer pTo
         * through a short link or through a long distance link
         */
        virtual bool isConnectedTo(Peer* pTo);

        /**
         * Update the display (text, position, etc.) of the peer on the canvas using the variables of the peer (id, status, etc.)
         * */
        virtual void updateDisplay();

        /**
         * If the peer is a member of the static network, these methods initialize it
         * */
        virtual void peerInizializationForStaticNetwork();
        virtual void createLongDistanceLinkForStaticNetwork();

        /**
         * Return the previous neighbor. With this neighbor the peer can calculate the range of ids that
         * it manages.
         * */
        Peer* getPrevNeighbor();

        /**
         * Return the next neighbor.
         * */
        Peer* getNextNeighbor();

        /**
         * Checks if the current peer is a manager for x
         * It uses the id of its predecessor, it doesn't need to send any message because the information
         * of the peer's neighbor are stored in the peer. (see 3.4 2nd paragraph. see 3.7.)
         * */
        virtual bool isManagerOf(double x);

        /**
         * Calculate the segment length
         * It uses the id of its predecessor, it doesn't need to send any message because the information
         * of the peer's neighbor are stored in the peer. (see 3.4 2nd paragraph. see 3.7.)
         * */
        virtual double getSegmentLength();

        /**
         * This is a *dynamic* implementation of the routing protocol.
         * It does NOT find the manager of x with global knowledge,
         * it rather starts the Symphony's routing protocol.
         * This method doesn't use the lookahead (see in the paper)
         * */
        virtual pair<Peer*,cGate*> getNextHopForKey(double x);

        /**
         * TODO comment
         */
        virtual void requestLookup(double x, CallbackType rp);

};

#endif
