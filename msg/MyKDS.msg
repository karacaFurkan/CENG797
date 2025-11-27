

// HelloMsg paket tanımı
packet HelloMsg
{
    int originId;
    int ttl;
    int seq;
}

// DominateMsg paket tanımı
packet DominateMsg
{
    int dominatorId;
}
