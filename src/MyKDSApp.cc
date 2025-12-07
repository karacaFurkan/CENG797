#include "MyKDSApp.h"
#include "MyKDS_m.h" 
#include <cmath>
#include <string>

Define_Module(MyKDSApp);

void MyKDSApp::initialize()
{
    // --- PARAMETRELER ---
    txRange = par("txRange").doubleValue();
    k = par("k").intValue();
    
    // --- RANDOM POZİSYON VE GÖRSELLİK ---
    if (par("x").doubleValue() == 0 && par("y").doubleValue() == 0) {
        double areaX = getParentModule()->par("areaX").doubleValue();
        double areaY = getParentModule()->par("areaY").doubleValue();
        x = uniform(0, areaX);
        y = uniform(0, areaY);
        par("x").setDoubleValue(x);
        par("y").setDoubleValue(y);
    } else {
        x = par("x").doubleValue();
        y = par("y").doubleValue();
    }
    
    // Konumu güncelle
    getDisplayString().setTagArg("p", 0, std::to_string(x).c_str());
    getDisplayString().setTagArg("p", 1, std::to_string(y).c_str());
    
    // Varsayılan görünüm (Mavi ve standart ikon)
    getDisplayString().setTagArg("i", 0, "device/pocketpc");
    getDisplayString().setTagArg("i", 1, ""); // Rengi temizle

    // --- DEĞİŞKENLER ---
    nodeId = getIndex();
    amDominator = false;
    isCovered = false; // YENİ: Başkası tarafından kapsanıyor muyum?
    seqCounter = 0;

    // Timer ayarları
    // Hello mesajları komşuluk tespiti içindir
    helloInterval = par("helloInterval").doubleValue() > 0 ? par("helloInterval") : 1.0;
    helloTimer = new cMessage("helloTimer");
    scheduleAt(simTime() + uniform(0, helloInterval), helloTimer);

    // Algoritma karar mekanizması
    // Herkes aynı anda karar vermesin diye nodeId'ye bağlı küçük bir sapma ekliyoruz
    // Bu "Lowest ID" veya öncelik mantığına zemin hazırlar.
    computeTimer = new cMessage("computeTimer");
    scheduleAt(simTime() + 2.0 + (nodeId * 0.01), computeTimer);
}

void MyKDSApp::handleMessage(cMessage *msg)
{
    if (msg == helloTimer) {
        sendHello();
        scheduleAt(simTime() + helloInterval, helloTimer);
    }
    else if (msg == computeTimer) {
        computeAndAnnounce();
        // Periyodik olarak tekrar kontrol et (ağ dinamikse)
        scheduleAt(simTime() + 2.0, computeTimer);
    }
    else {
        if (dynamic_cast<HelloMsg*>(msg)) {
             processHello(msg); 
             delete msg; 
        }
        else if (dynamic_cast<DominateMsg*>(msg)) {
            // ÖNEMLİ: Dominator mesajını işle
            processDominateMsg(msg);
            delete msg;
        }
        else {
            delete msg;
        }
    }
}

void MyKDSApp::sendHello()
{
    // Hello mesajı sadece komşulukları keşfetmek içindir
    HelloMsg* msg = new HelloMsg();
    msg->setOriginId(nodeId);
    msg->setTtl(1); // Hello sadece 1 hop gitsin (komşumu bileyim yeter)
    msg->setSeq(seqCounter++);
    msg->setName("Hello");
    broadcastToNeighbors(msg);
}

void MyKDSApp::processHello(cMessage* msg)
{
    // Komşuluk listesi tutmak isterseniz burayı kullanabilirsiniz
    // Şu anki basit K-hop algoritması için kritik değil ama dursun.
    HelloMsg* h = check_and_cast<HelloMsg*>(msg);
    int origin = h->getOriginId();
    bestKnownHop[origin] = 1; 
}

// YENİ FONKSİYON: Gelen Dominator İlanını İşle
void MyKDSApp::processDominateMsg(cMessage* msg)
{
    DominateMsg* d = check_and_cast<DominateMsg*>(msg);
    int domId = d->getDominatorId();
    int msgTTL = d->getTtl(); // Mesajın kalan ömrü

    // Eğer zaten Dominator isem veya mesaj kendi mesajımsa yoksay
    if (amDominator || domId == nodeId) return;

    // Eğer daha önce kapsanmadıysam, artık kapsandım!
    if (!isCovered) {
        isCovered = true;
        
        // --- GÖRSELLİK: Kapsanan Node YEŞİL olsun ---
        getDisplayString().setTagArg("i", 1, "green"); 
        
        EV << "Node " << nodeId << " is now covered by Dominator " << domId << "\n";
    }

    // --- FORWARDING (K-HOP MANTIĞI) ---
    // Eğer mesajın ömrü varsa (TTL > 0), bir sonraki halkaya ilet
    if (msgTTL > 0) {
        // Daha önce bu dominator'dan bu mesajı almadıysam veya daha kısa yoldan geldiyse ilet
        // (Basitlik için flood korumasını es geçiyoruz, simülasyon küçükse sorun olmaz)
        
        DominateMsg* forwardedMsg = d->dup();
        forwardedMsg->setTtl(msgTTL - 1);
        broadcastToNeighbors(forwardedMsg);
    }
}

void MyKDSApp::computeAndAnnounce()
{
    // Eğer zaten dominator isem veya başkası beni kapsamışsa (slave isem) bir şey yapma
    if (amDominator || isCovered) {
        return;
    }

    // Eğer buraya geldiysem: Ne Dominator'ım ne de bir Dominator beni kapsadı.
    // O zaman inisiyatif alıp Dominator oluyorum!
    
    amDominator = true;
    isCovered = true; // Kendim tarafından kapsandım

    // --- GÖRSELLİK: Dominator KIRMIZI ve SERVER ikonu olsun ---
    getDisplayString().setTagArg("i", 0, "device/server");
    getDisplayString().setTagArg("i", 1, "red");

    EV << "Node " << nodeId << " declared itself DOMINATOR!\n";

    // Tüm ağa (k-hop kadar) Dominator olduğumu duyur
    DominateMsg* msg = new DominateMsg();
    msg->setDominatorId(nodeId);
    msg->setTtl(k - 1); // Ben 0. hop'um, komşum 1. hop. TTL k-1 kadar daha gidecek.
    msg->setName("I_AM_DOMINATOR");
    broadcastToNeighbors(msg);
}

void MyKDSApp::broadcastToNeighbors(cMessage* msg)
{
    cModule* net = getParentModule();
    int n = net->par("numNodes").intValue(); 

    for (int i = 0; i < n; i++) {
        cModule* destMod = net->getSubmodule("host", i);
        if (!destMod || destMod == this) continue;

        double tx = par("txRange").doubleValue();
        double targetX = destMod->par("x").doubleValue();
        double targetY = destMod->par("y").doubleValue();

        double dist = std::sqrt(std::pow(x - targetX, 2) + std::pow(y - targetY, 2));

        if (dist <= tx) {
            sendDirect(msg->dup(), destMod, "in");
        }
    }
    delete msg;
}

void MyKDSApp::finish() {
    // İstatistikler buraya
}
