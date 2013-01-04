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

#include "peer.h"
#include "Msgs_m.h"
#include <string>
#include <sstream>
#include <cassert>
#include <cmath>
#include <assert.h>

using namespace std;

Define_Module(Peer);

bool Peer::connect(Peer* pFrom, Peer* pTo, long linkType) {

    //The number of incoming links per node is bounded by the upper limit of 2k.
    if (pTo->gateSize("longDistanceLink") >= 2 * (int)par("k")) return false;

    //check if the two peer is already connected
    if (areConnected(pFrom, pTo)) return false;

    //Channel creation
    cChannelType *channelType = cChannelType::get((linkType & longDistanceLink) ? "symphony.LongDistanceLinkChannel" : "symphony.ShortLinkChannel");
    ostringstream channelNameStream;
    channelNameStream << ((linkType & longDistanceLink) ? "channelLong_" : "channelShort_") << pFrom->id << "_" << pTo->id;
    cChannel *channel = channelType->create(channelNameStream.str().c_str());
    channelNameStream.clear();
    channelNameStream << ((linkType & longDistanceLink) ? "channelLong_" : "channelShort_") << pTo->id << "_" << pFrom->id;
    cChannel *channelRev = channelType->create(channelNameStream.str().c_str());

    //Connect the nodes
    if  (linkType == longDistanceLink) {
        pFrom->setGateSize("longDistanceLink", pFrom->gateSize("longDistanceLink") + 1);
        pTo->setGateSize("longDistanceLink", pTo->gateSize("longDistanceLink") + 1);
        pFrom->gate("longDistanceLink$o", pFrom->gateSize("longDistanceLink") - 1)->connectTo(
            pTo->gate("longDistanceLink$i", pTo->gateSize("longDistanceLink") - 1), channel, false);
        if(!getParentModule()->par("unidirectional").boolValue()) {
            pTo->gate("longDistanceLink$o", pTo->gateSize("longDistanceLink") - 1)->connectTo(
                pFrom->gate("longDistanceLink$i", pFrom->gateSize("longDistanceLink") - 1), channelRev, false);
        }
    } else {
        if (linkType & shortLinkSucc) {
            pFrom->gate("shortLink$o", 1)->connectTo(pTo->gate("shortLink$i", 0), channel, false);
            pTo->gate("shortLink$o", 0)->connectTo(pFrom->gate("shortLink$i", 1), channelRev, false);
        } else {
            pFrom->gate("shortLink$o", 0)->connectTo(pTo->gate("shortLink$i", 1), channel, false);
            pTo->gate("shortLink$o", 1)->connectTo(pFrom->gate("shortLink$i", 0), channelRev, false);
        }
    }

    return true;
}

bool Peer::connectTo(Peer* pTo, long linkType) {
    return connect(this, pTo, linkType);
}

bool Peer::disconnect(Peer* pFrom, Peer* pTo) {
    if (!areConnected(pFrom, pTo)) return false;

    /* TODO
     * Se si trova un gate che collega pFrom con pTo si distrugge il gate (vedere bene documentazione, per comprendere i side effect)
     * */
    return false;
}

bool Peer::disconnectLinkTo(Peer* pTo) {
    return disconnect(this, pTo);
}

pair<Peer*,cGate*> Peer::getNextHopForKey(double x) {
    assert (! this->isManagerOf(x));

    Peer* bestPeer = NULL;
    cGate* bestGate = NULL;
    double currBest = -1.0;

    cGate* gate;
    for (cModule::GateIterator i(this); !i.end(); i++) {
        gate = i();

        // Iterate all *output* links
        if (gate->getType() == cGate::OUTPUT) {
            if (gate->isConnected()) {
                Peer* neighbor = dynamic_cast<Peer*>(gate->getNextGate()->getOwnerModule());

                double test;
                if (getParentModule()->par("unidirectional").boolValue())
                    test = x - neighbor->id;
                else
                    test = abs(x - neighbor->id);

                if (currBest == -1.0 || (test >= 0  && test < currBest)) {
                    bestPeer = neighbor;
                    bestGate = gate;
                    currBest = test;
                }
            }
        }

    }

    return pair<Peer*,cGate*>(bestPeer, bestGate);
}

/**
 * TODO
 * startLookup e lookup si possono unire in un unico metodo se si toglie pending_lookup_key
 * */
void Peer::startLookup(double x) {
    this->pending_lookup_key = x;
    lookup(x, this, 0);
}

void Peer::lookup(double x, Peer* sender, int hops) {
    if (this->isManagerOf(x)) {
        ManagerMsg* msg = new ManagerMsg();
        msg->setManagerID(getId());
        msg->setX(x);
        msg->setHops(hops);
        sendDirect(msg, sender, "directin");
    }
    else {
        LookupMsg* msg = new LookupMsg();
        msg->setSenderID(sender->getId());
        msg->setX(x);
        msg->setHops(hops);
        send(msg, getNextHopForKey(x).second);
    }
}

bool Peer::areConnected(Peer* pFrom, Peer* pTo) {

    if (pFrom == pTo) return true;

    cGate* gate;
    for (cModule::GateIterator i(pFrom); !i.end(); i++) {
        gate = i();

        // We analyze only the output short links and the long distance links
        if (gate->getType() == cGate::OUTPUT){
            // If the gate is connected with the pTo, it return true
            if (gate->isConnected()) {
                if(gate->getNextGate()->getOwnerModule() == pTo) return true;
            }
        }

    }
    return false;
}

bool Peer::isConnectedTo(Peer* pTo) {
    return areConnected(this, pTo);
}

Peer* Peer::getPrevNeighbor() {
   return dynamic_cast<Peer*>(gate("shortLink$o", 0)->getNextGate()->getOwnerModule());
}

Peer* Peer::getNextNeighbor() {
   return dynamic_cast<Peer*>(gate("shortLink$o", 1)->getNextGate()->getOwnerModule());
}

bool Peer::isManagerOf(double x) {
    Peer* previous = getPrevNeighbor();

    // case 0: THIS == X
    if (x == id) return true;

    // case 1: |--..--PREVIOUS--X--THIS--...--|
    if ((previous->id < x) && (x < id)) return true;

    // case 2: |--X--THIS--...--PREVIOUS--|
    if ((x < id) && (id < previous->id)) return true;

    // case 3: |--THIS--...--PREVIOUS--X--|
    if ((x > previous->id) && (id < previous->id)) return true;

    return false;
}

double Peer::getSegmentLength() {
    Peer* previous = getPrevNeighbor();

    // case 0: THIS == X
    if (previous->id == id) return 0;

    // case 1: |--..--PREVIOUS++THIS--...--|
    if (previous->id < id) return id - previous->id;

    // case 2: |++THIS--...--PREVIOUS++|
    if (id < previous->id) return id + 1.0 - previous->id;

    return false;
}

void Peer::updateDisplay() {
    char buf[64];
    sprintf(buf, "%lf : %lf : %lf", id, getPrevNeighbor()->id, getSegmentLength());
    getDisplayString().setTagArg("t", 0, buf);
}

void Peer::peerInizializationForStaticNetwork() {

    //Estimation of n for the STATIC network (remember that, in this case, n is accurate. It's static!)
    n = (int)getParentModule()->par("n");

    //ID initialization for the STATIC network
    id = (double)par("id");

    //Short Link Creation for the STATIC network
    /* We create it in the ned file, but for completeness this is the code */
    /* Peer* nextBrother = dynamic_cast<Peer*>(getParentModule()->getSubmodule(getName(), (getIndex() + 1) % n));
     connectTo(nextBrother, shortLink | shortLinkSucc);
     nextBrother->connectTo(this, shortLink | shortLinkPrev); */

    /* Long Distance Link Creation for the STATIC network
     * We need that all the initialization is done. This can be done with scheduleAt */
    scheduleAt(simTime() + uniform(0,0.01), new cMessage("createLongDistanceLinksForStaticNetwork"));
    //createLongDistanceLinkForStaticNetwork();

}

void Peer::createLongDistanceLinkForStaticNetwork(){

    int attempts = 0;

    //We must create k long distance links, no more.
    if (gateSize("longDistanceLink") >= (int)par("k")) return;

    while (attempts < (int)par("attemptsUpperBound")){

        // see the paper for this formula
        double rndId = exp(log(n) * (drand48() - 1.0));

        //find the manager of the random number, in the God's way :)
        Peer* rndIdManager = NULL;
        for (cModule::SubmoduleIterator i(getParentModule()); !i.end(); i++) {
           if (strcmp(i()->getName(), getName()) == 0){
               Peer* peer = dynamic_cast<Peer*>(i());
               if (peer->isManagerOf(rndId)) {
                   rndIdManager = peer;
                   break;
               }
           }
        }

        //If we can't find a manager, we try with another number
        if (rndIdManager == NULL) {
            attempts++;
            continue;
        }

        //try to connect to the manager, if we can't connect to this node, increase the attempts
        if (connectTo(rndIdManager, longDistanceLink)) {
            scheduleAt(simTime() + uniform(0,0.01), new cMessage("createLongDistanceLinksForStaticNetwork"));
            return;
        } else {
           attempts++;
        }

    }


}

void Peer::initialize() {

    //If I am a member of a static network we initialize the connections at once.
    if (par("isMemberOfAStaticNetwork").boolValue()) peerInizializationForStaticNetwork();

    this->pending_lookup_key = -1;

}

void Peer::handleMessage(cMessage *msg) {

    if (msg->isName("createLongDistanceLinksForStaticNetwork")) {
        createLongDistanceLinkForStaticNetwork();
        updateDisplay();
    }

    else if (strcmp(msg->getClassName(), "LookupMsg") == 0) {
        LookupMsg* luMsg = check_and_cast<LookupMsg*>(msg);
        //TODO: Controllare il caso in cui getActiveSimulation()->getModule non fallisca
        Peer* sender = dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(luMsg->getSenderID()));
        double x = luMsg->getX();
        int hops = luMsg->getHops() + 1;

        lookup(x, sender, hops);
    }

    else if (strcmp(msg->getClassName(), "ManagerMsg") == 0) {
        ManagerMsg* mMsg = check_and_cast<ManagerMsg*>(msg);
        //TODO: Controllare il caso in cui getActiveSimulation()->getModule non fallisca
        Peer* manager = dynamic_cast<Peer*>(cSimulation::getActiveSimulation()->getModule(mMsg->getManagerID()));
        double x = mMsg->getX();
        int hops = mMsg->getHops();

        // TODO Il manager di x ha risposto!
        // Ora dovresti: (1) vedere se avevi veramente fatto richiesta
        // (2) se sì, passare allo step successivo di computazione (es. prossima richiesta)

        if (this->pending_lookup_key == -1) {
            // Non avevi richiesto nulla
            // E' un evento che si può mai verificare?
            throw -1;
        }
        else if (this->pending_lookup_key == x) {
            // Avevi proprio richiesto x e ti è arrivato. OK!

            // ORA FORSE SAREBBE COMODO INSTAURARE UN CANALE DI COMUNICAZIONE
            // DIRETTO CON MANAGER?
            // Il lookup viene chiamato sia per stabilire un long, per fare un join, per fare una query (nel nostro caso dobbiamo verificare solamente che � stato trovato un manager, cio� che la nostra rete di overlay � corretta)
            // Ci vuole un modo per restituire il risultato

            // RISP: Direi per ora di ignorare eventuali errori bizantini, come dici nel commento di sotto.
            // Lasciamo però i due "if" per i casi sfigati (quello sopra e quello sotto di questo), mettendoci dentro
            // due throw exception, giusto per esser certi che non accadano mai quei casi durante il debug.
            // Se alla fine il problema non si verifica mai, togliamo pending_lookup_key e uniamo startLookup e lookup.

            /*
             * La tua RISP non risp alla alla domand di sopra :D. Cmq RISP della RISP:
             *
             * Potrebbe verificarsi che mi arrivi la risposta di un lookup che non avevo richiesto, ma adesso con l'id
             * nei messaggi invece che con il puntatore è molto raro che si verifichi. Solo quando si addormenta il peer dopo
             * avere fatto una richiesta e poi si risveglia prima che sia arrivata la risposta.
             * Ma quello di sotto invece non si verificherà mai.
             *
             * Io direi di togliere il pending_lookup_key, unire startLookup e lookup, ed inventarsi un modo per ritornare
             * il risultato alle funzioni che hanno richiesto il lookup.
             *
             * Visto che alla fine sono circa tre le funzioni che possono richiedere il lookup, io direi di fare un po' come
             * ho fatto anche nella createLongDistanceLinkForStaticNetwork:
             *
             *  - se sono A, B e C le funzioni che possono chiamare il lookup
             *  - le funzioni A B e C devono essere fatte in modo che terminano appena c'è qualcosa che va storto
             *    per esempio quando non sanno chi è il manager di un certo id
             *  - se la funzione A vuole sapere il manager di un certo id chiama una funzione (che dobbiamo definire)
             *    che ricerca per prima cosa il manager guardando tutti i link uscenti, ed anche su una lista di nodi,
             *    una sorta di cache (che dobbiamo definire). Se questa funzione ritorna un esito negativo la funzione A
             *    fa la richiesta di lookup e poi termina.
             *  - la funzione A quando chiama il lookup mette il proprio identificativo come parametro della funzione, per esempio "A"
             *  - il lookup mette nel messaggio anche questo parametro "A", in modo che quando viene ricevuta la risposta del lookup
             *    in questo esatto punto venga messa la coppia (id, manager) nella lista famosa e venga richiamata la funzione che era terminata.
             *
             *
             *    E' un po' bruttino mettere la funzione da richiamare come parametro,
             *    ma altrimenti dovremmo creare una lista delle funzioni in attesa ed al ricevimento di
             *    una risposta di lookup devono essere risvegliate tutte...
             *
             *    Se vogliamo però anche inglobare il primo if, bisogna creare questa lista, che ha uan forma di
             *    (id, funzione chiamante). In questo punto esatto bisogna
             *    che vengano chiamate tutte le funzioni in attesa per quell'id,
             *    poi togliere le funzioni che vengono chiamate dalla questa lista.
             *    Se non si trovano funzioni l'elemento viene ignorato.
             *
             */

            this->pending_lookup_key = -1;
        }
        else {
            // Non ha risposto lookup per x... errore o lo permettiamo?
            // Succede tutte le volte che si chiama la startLookup più volte prima di attendere una risposta
            // Io direi di togliere questo pending_lookup_key, le reti sono affidabili e non ci sono utenti maliziosi, e il peer non fa cose strane
            throw -2;
            this->pending_lookup_key = -1;
        }
    }

}
