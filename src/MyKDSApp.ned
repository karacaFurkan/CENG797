package my_kds_project;

simple MyKDSApp
{
    parameters:
        double txRange;
        int k;
        double helloInterval;
        double x = default(0);
        double y = default(0);
    gates:
        input in;
        output out;
}
