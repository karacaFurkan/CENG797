#ifndef __MYKDSAPP_H_
#define __MYKDSAPP_H_

#include <omnetpp.h>
#include <map>

using namespace omnetpp;

/**
 * Ad-hoc ağlar için k-hop Dominating Set algoritması simülasyonu.
 */
class MyKDSApp : public cSimpleModule
{
  private:
    // --- Modül Parametreleri ---
    int nodeId;             // Düğümün ID'si
    double txRange;         // İletişim menzili
    int k;                  // k-hop değeri
    double x, y;            // Konum koordinatları
    double helloInterval;   // Hello mesajı periyodu

    // --- Durum Değişkenleri ---
    bool amDominator;       // Ben bir Dominator (Cluster Head) miyim?
    bool isCovered;         // Başka bir Dominator tarafından kapsanıyor muyum?
    int seqCounter;         // Mesaj sıra numarası

    // --- Timer Mesajları ---
    cMessage *helloTimer;   // Komşuluk keşfi için zamanlayıcı
    cMessage *computeTimer; // Algoritma karar mekanizması için zamanlayıcı

    // --- Veri Yapıları ---
    // Komşuları ve hop uzaklıklarını tutmak için map
    // Key: NodeID, Value: Hop Count (veya TTL)
    std::map<int, int> bestKnownHop;

  protected:
    // OMNeT++ Standart Fonksiyonları
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    // --- Yardımcı Fonksiyonlar ---
    
    // Hello mesajı oluşturur ve yayar
    void sendHello();
    
    // Gelen Hello mesajını işler (komşuluk keşfi)
    void processHello(cMessage* msg);
    
    // Gelen Dominator ilan mesajını işler (kapsanma durumu ve forwarding)
    void processDominateMsg(cMessage* msg);
    
    // Algoritmayı çalıştırır: Dominator olup olmayacağına karar verir
    void computeAndAnnounce();
    
    // Mesajı menzil içindeki tüm komşulara "sendDirect" ile iletir
    void broadcastToNeighbors(cMessage* msg);
};

#endif
