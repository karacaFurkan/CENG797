

packet HelloMsg {
    int originId;
    int ttl;
    int seq;
}

packet DominateMsg {
    int dominatorId;
    int ttl;  // <-- BU ALANI EKLEYİN
}
