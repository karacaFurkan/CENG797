package my_kds_project;

network KDSNetwork
{
    parameters:
        int numNodes = default(10); // Varsayılan değer ekledim
        double areaX = default(500);
        double areaY = default(500);
        double txRange = default(100);
        int k = default(2);
    submodules:
        host[numNodes]: MyKDSApp {
            parameters:
                txRange = parent.txRange; // Network parametresini al
                k = parent.k;
                // x ve y burada rastgele atanabilir veya C++ içinde halledilir
                @display("p=,,m,5,5,5"); // İkon yerleşimi
        }
        // connections bloğu SİLİNDİ. Ad Hoc ağlarda sendDirect kullanılır.
}
