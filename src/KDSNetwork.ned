package my_kds_project;




network KDSNetwork
{
    parameters:
        int numNodes;
        double areaX;
        double areaY;
        double txRange;
        int k;
    submodules:
        host[numNodes]: MyKDSApp {
            parameters:
                txRange = default(200);
                k = default(2);
        }
    connections allowunconnected:
        for i=0..numNodes-2 {
            host[i].out --> host[i+1].in;
        }
}
