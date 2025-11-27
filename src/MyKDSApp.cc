#include "MyKDSApp.h"
#include "../msg/MyKDS_m.cc"

Define_Module(MyKDSApp);

void MyKDSApp::initialize()
{
    txRange = par("txRange").doubleValue();
    k = par("k").intValue();
    x = par("x").doubleValue();
    y = par("y").doubleValue();

    nodeId = getIndex();
    amDominator = false;
    controlBytes = 0;
    seqCounter = 0;

    helloInterval = par("helloInterval").doubleValue() > 0 ? par("helloInterval") : 1.0;

    helloTimer = new cMessage("helloTimer");
    scheduleAt(simTime() + uniform(0, helloInterval), helloTimer);

    computeTimer = new cMessage("computeTimer");
    scheduleAt(simTime() + 2.0, computeTimer);
}

void MyKDSApp::handleMessage(cMessage *msg)
{
    if (msg == helloTimer) {
        sendHello();
        scheduleAt(simTime() + helloInterval, helloTimer);
    }
    else if (msg == computeTimer) {
        computeAndAnnounce();
        scheduleAt(simTime() + 5.0, computeTimer);
    }
    else {
        if (HelloMsg* h = dynamic_cast<HelloMsg*>(msg)) {
            processHello(msg);
            delete msg;
        }
        else if (DominateMsg* d = dynamic_cast<DominateMsg*>(msg)) {
            int dom = d->getDominatorId();
            bestKnownHop[dom] = 0;
            getDisplayString().setTagArg("i",0, "status/ok");
            delete msg;
        }
        else {
            delete msg;
        }
    }
}

// minimal s-hop dominating set (k-hop) skeleton
void MyKDSApp::sendHello()
{
    HelloMsg* msg = new HelloMsg();
    msg->setOriginId(nodeId);
    msg->setTtl(k);
    msg->setSeq(seqCounter++);
    broadcastToNeighbors(msg);
}

void MyKDSApp::processHello(cMessage* msg)
{
    HelloMsg* h = check_and_cast<HelloMsg*>(msg);
    int origin = h->getOriginId();
    int ttl = h->getTtl();

    // güncel hop bilgisi tut
    if (bestKnownHop.find(origin) == bestKnownHop.end() || bestKnownHop[origin] > ttl) {
        bestKnownHop[origin] = ttl;
    }
}

void MyKDSApp::computeAndAnnounce()
{
    // minimal s-hop dominating set algoritması
    // node kendisini dominator olarak seçebilir
    if (!amDominator) {
        int covered = 0;
        for (auto& kv : bestKnownHop) {
            if (kv.second <= k) covered++;
        }
        if (covered < (int)bestKnownHop.size()) {
            amDominator = true;
            DominateMsg* msg = new DominateMsg();
            msg->setDominatorId(nodeId);
            broadcastToNeighbors(msg);
            getDisplayString().setTagArg("i",0,"status/active");
        }
    }
}

void MyKDSApp::finish()
{
    EV << "Simulation finished for node " << nodeId << "\n";
}

// OMNeT++ 6.2.0 compliant broadcast
void MyKDSApp::broadcastToNeighbors(cMessage* msg)
{
    cModule* net = getParentModule();
    cModule* firstNode = net->getSubmodule("node",0);
    int n = firstNode->getVectorSize(); // node vector size
    for (int i=0;i<n;i++) {
        cModule* mod = net->getSubmodule("node", i);
        if (mod==this) continue;
        send(msg->dup(),"out",i); // assumes simple direct connection
    }
    delete msg;
}
