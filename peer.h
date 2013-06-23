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

#define DEBUG_LOOKUP
#define DEBUG_JOIN
#define DEBUG_LEAVE
//#define DEBUG_RELINKING
#define DEBUG_CREATELONGLINK

#ifndef __SYMPHONY_PEER_H_
#define __SYMPHONY_PEER_H_

#include <omnetpp.h>
#include <utility>
using namespace std;

class Peer : public cSimpleModule {

private:

    /**
     * The type of the link
     */
    enum LinkType {shortLink = 0x0, longDistanceLink = 0x2};

    /**
     * The type of the short link: it can be the link to the previous neighbor or to the successor
     */
    enum ShortLinkType {shortLinkPrev = 0x0, shortLinkSucc = 0x1};

    typedef void (Peer::*lookupCallbackPointer)(Peer*);

    enum LookupSpecialization {lookupNoSpecialization, lookupJoinSpecialization};

    typedef struct {
        double key;
        lookupCallbackPointer callback; //determines the callback
    } PendingLookup;

public:
    /**
     * The state of the peer
     */
    enum State {Idle, Joining, Linking, ReLinking, Connected, Leaving};

    State state;

    /* To request a join, this can be called by the peer that wants to join to the network
    *
    * This method chooses its own id uniformly at random. Then, using the routing protocol
    * (i.e. it requests a lookup), it identifies the manager of this id.
    * Since the node is not connected with the other peers, it use a known peer
    * to start the lookup.
    * */
   virtual void requestJoin();

   /* To request that the peer leave the network
    * */
   virtual void requestLeave();

   virtual void saveCreateLongDistanceLinksExternalScheduledEvents(cMessage* event);

protected:

    unsigned int n; //Number of peers in the network, it can be an extimation
    double id; //Own id. For the STATIC network the id can be found in the parameters

    /* knownPeer is a random known peer that is in the network (see the paper).
     * For us is a random static peer, because the static peers are always connected.
     * It is used, for example, if we are a fresh peer, without any output link, and
     * we can't forward any message without using it. */
    int knownPeer_idx;

    // -----------------------------------------------------------------
    // CONNECTIONS MANAGEMENT
    // -----------------------------------------------------------------

   /***
    * Connect the peer pFrom to the peer pTo
    *
    * @param linkType specifies the type of link (short of long, and if short if it is a prev or next link)
    * For example, to specify that the link is a short link, and it is the link to the previous neighbor
    * you can write shortLink | shortLinkPrev
    * */
   virtual void connect(Peer* pFrom, Peer* pTo, long linkType);

   /***
    * Connect the current peer to the peer pTo
    *
    * @param linkType specifies the type of link (short of long, and if short if it is a prev or next link)
    * For example, to specify that the link is a short link, and it is the link to the previous neighbor
    * you can write shortLink | shortLinkPrev
    * */
   virtual void connectTo(Peer* pTo, long linkType);

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
   virtual bool disconnectLinksTo(Peer* pTo);

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

   virtual float percentageLongLinks();

   /**
    * Number of Connected gates
    */
   virtual unsigned int getNumberOfConnectedLongLinkGates();

   /**
    * Create all the long distance links the peer needs. The first time it must be called without any parameters
    * The method itself calls the lookup, and when a lookup result become, is the lookup manager that calls again
    * this method with the appropriate parameters.
    */
   virtual void createLongDistanceLinks(Peer* lookupResult);
   double createLongDistanceLinks_rndId;
   int createLongDistanceLinks_attempts;
   cMessage* createLongDistanceLinks_scheduledEvent;
   vector<cMessage*> createLongDistanceLinks_external_scheduledEvents;

   // -----------------------------------------------------------------
   // JOIN
   // -----------------------------------------------------------------


   double joinRequestedId;

   /* When the lookup (requested by the peer that wants to join) arrives to the manager of the random id.
    * the manager calls this method to join the peers.
    * This method perform an atomic procedure that modify the connections of: the peer that wants to join,
    * the manager itself and the previous peer. This method does the n estimation too, and updates the n
    * of the involved peers.
    * */
   virtual void join(Peer*, double requestedId);

   /* This is called after the manager join.
    * */
  virtual void requestJoinCallback(Peer* manager);

   // -----------------------------------------------------------------
   // LEAVE
   // -----------------------------------------------------------------



   // -----------------------------------------------------------------
   // N-Estimation
   // -----------------------------------------------------------------
   virtual void manageNUpdate(unsigned int new_n);

   // -----------------------------------------------------------------
   // LOOKUP
   // -----------------------------------------------------------------

   /**
    * This is a *dynamic* implementation of the routing protocol.
    * It does NOT find the manager of x with global knowledge,
    * it rather starts the Symphony's routing protocol.
    * This method doesn't use the lookahead (see in the paper)
    * */
   virtual pair<Peer*,cGate*> getNextHopForKey(double x);

   /**
    * request a lookup
    * the callback will be called when arrive a response for the lookup.
    * In the last case, the callback will be called with the manager set to NULL.
    */
   virtual void requestLookup(double x, lookupCallbackPointer callback, LookupSpecialization ls);
   simsignal_t lookupHopsSignal;
   simsignal_t lookupStabilitySignal;
   simsignal_t lookupTimeSignal;
   simsignal_t lookupPercLongLinksSignal;
   //simsignal_t NSignal;
   map<unsigned long, PendingLookup>* pendingLookupRequests;
   unsigned long lookup_requestIDInc;

   // -----------------------------------------------------------------
   // UTILITY
   // -----------------------------------------------------------------

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
    * Return the previous neighbor. With this neighbor the peer can calculate the range of ids that
    * it manages.
    * */
   virtual Peer* getPrevNeighbor();

   /**
    * Return the next neighbor.
    * */
   virtual Peer* getNextNeighbor();

   // -----------------------------------------------------------------
   // GRAPHICS
   // -----------------------------------------------------------------

    /**
     * Update the display (text, position, etc.) of the peer on the canvas using the variables of the peer (id, status, etc.)
     * */
    virtual void updateDisplay(bool displayId);

   // -----------------------------------------------------------------
   // INITIALIZATION
   // -----------------------------------------------------------------

    virtual void initialize();
    virtual void resetPeerState();
    virtual void longDistanceLinksInitialization();

   // -----------------------------------------------------------------
   // MESSAGE MANAGEMENT
   // -----------------------------------------------------------------

    virtual void handleMessage(cMessage *msg);

};

#endif
