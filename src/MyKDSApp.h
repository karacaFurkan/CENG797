#include <omnetpp.h>
#include <map>


using namespace omnetpp;

class MyKDSApp : public cSimpleModule
{
  private:
    int nodeId;
    double txRange;
    int k; // s-hop
    double x, y;
    bool amDominator;
    int seqCounter;
    double helloInterval;
    int controlBytes;

    cMessage *helloTimer;
    cMessage *computeTimer;
    std::map<int,int> bestKnownHop;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    void sendHello();
    void processHello(cMessage* msg);
    void computeAndAnnounce();
    void broadcastToNeighbors(cMessage* msg);
};
