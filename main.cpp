#include "csr_mac_core.hpp"
#include <iostream>

int main()
{
    CsrMacCore mac;
    mac.SetNow(0.0);

    // fake a couple of neighbors using slots 5 and 10
    auto& ng = mac.Neighbors();
    NeighborEntry a; a.node_addr = 1; a.rtslot_counter = 5;  ng.push_back(a);
    NeighborEntry b; b.node_addr = 2; b.rtslot_counter = 10; ng.push_back(b);

    for (int k = 0; k < 5; ++k) {
        int slot = mac.PickTxSlot(/*estimated_active_nodes*/ 8, /*rn_range*/ 0);
        std::cout << "Picked TX slot: " << slot << "\n";
    }

    int renewed = mac.RenewTxSlot(5);
    std::cout << "Renewed slot from 5 to: " << renewed << "\n";

    return 0;
}
