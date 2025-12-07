package my_kds_project;

simple MyKDSApp
{
    parameters:
        double txRange;
        int k;
        double helloInterval;
        double x @mutable = default(0); 
        double y @mutable = default(0);
    gates:
        input in @directIn;
        //output out;
}
