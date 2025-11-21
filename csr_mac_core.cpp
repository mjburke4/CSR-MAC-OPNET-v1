#include "csr_mac_core.hpp"
#include <algorithm>

// simple global RNG
static std::mt19937& GlobalRng()
{
    static std::mt19937 rng{std::random_device{}()};
    return rng;
}

CsrMacCore::CsrMacCore(const Config& cfg)
    : cfg_(cfg)
{
    tslot_table_.resize(cfg_.max_slot_reserve + 1);
    for (int i = 0; i <= cfg_.max_slot_reserve; ++i) {
        tslot_table_[i].slot      = i;
        tslot_table_[i].node_addr = -1;
        tslot_table_[i].reserved  = false;
    }
}

int CsrMacCore::SlotRangeForNodes(int nodes) const
{
    int slot_range;
    switch (nodes) {
        case 0:  slot_range = 15;  break;
        case 1:  slot_range = 18;  break;
        case 2:  slot_range = 21;  break;
        case 3:  slot_range = 25;  break;
        case 4:  slot_range = 31;  break;
        case 5:  slot_range = 37;  break;
        case 6:  slot_range = 44;  break;
        case 7:  slot_range = 52;  break;
        case 8:  slot_range = 63;  break;
        case 9:  slot_range = 75;  break;
        case 10: slot_range = 89;  break;
        case 11: slot_range = 106; break;
        case 12: slot_range = 127; break;
        case 13: slot_range = 151; break;
        case 14: slot_range = 180; break;
        case 15: slot_range = 214; break;
        case 16: slot_range = 255; break;
        default: slot_range = 255; break;
    }
    return slot_range;
}

int CsrMacCore::PickTxSlot(int estimated_active_nodes, int rn_range)
{
    int slot_range = SlotRangeForNodes(estimated_active_nodes);

    if (rn_range > 0 && last_tx_time_ > 0.0) {
        slot_range = rn_range;
    }

    if (cfg_.enable_reserve_tslot) {
        for (auto& entry : tslot_table_) {
            entry.reserved = false;
            entry.node_addr = -1;
        }
        for (const auto& n : neighbors_) {
            int j = n.rtslot_counter;
            if (j >= 0 && j < static_cast<int>(tslot_table_.size())) {
                tslot_table_[j].reserved = true;
                tslot_table_[j].node_addr = n.node_addr;
            }
        }

        std::uniform_int_distribution<int> dist(1, slot_range);
        int j = dist(GlobalRng());

        int i = -1;
        int checked = 0;
        TslotEntry* chosen = nullptr;
        while (checked < static_cast<int>(tslot_table_.size())) {
            i = (i + 1) % static_cast<int>(tslot_table_.size());
            ++checked;
            auto& entry = tslot_table_[i];
            if (!entry.reserved) {
                if (j == 0) {
                    chosen = &entry;
                    break;
                }
                --j;
            }
        }

        if (!chosen) {
            std::uniform_int_distribution<int> dist2(1, slot_range);
            int idx = dist2(GlobalRng());
            chosen = &tslot_table_[idx];
        }

        int slot = chosen->slot;
        if (rn_range >= 0 && last_tx_time_ > 0.0 && slot > total_tslot_) {
            total_tslot_ = slot;
        }
        txslot_ = slot;
        txslot_counter_ = slot;
        return slot;
    } else {
        std::uniform_int_distribution<int> dist(1, slot_range);
        int idx  = dist(GlobalRng());
        int slot = tslot_table_[idx].slot;
        if (rn_range >= 0 && last_tx_time_ > 0.0 && slot > total_tslot_) {
            total_tslot_ = slot;
        }
        txslot_ = slot;
        txslot_counter_ = slot;
        return slot;
    }
}

int CsrMacCore::RenewTxSlot(int islot)
{
    int slot = islot;
    while (true) {
        bool conflict = false;

        if (txslot_counter_ - 1 == slot) {
            conflict = true;
        }
        for (const auto& n : neighbors_) {
            if (n.rtslot_counter == slot) {
                conflict = true;
                break;
            }
        }
        if (conflict) {
            ++slot;
            if (slot >= static_cast<int>(tslot_table_.size())) {
                slot = 0;
            }
        } else {
            break;
        }
    }
    txslot_ = slot;
    txslot_counter_ = slot;
    return slot;
}
