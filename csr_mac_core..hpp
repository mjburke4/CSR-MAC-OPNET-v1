#pragma once

#include <cstdint>
#include <vector>
#include <random>

struct NeighborEntry {
    int      node_addr      = -1;
    int      active         = 0;
    double   tx_power       = 0.0;
    double   S0_power       = 0.0;
    double   pathloss       = 0.0;
    double   last_rcvd_time = 0.0;
    int      flow_ctrl_counter = 0;
    int      flow_ctrl_threshold = 0;
    int      flow_ctrl_spad = 0;
    int      relay_holdoff  = 0;
    int      cc_ack_count   = 0;
    int      last_sent_seq  = 0;
    int      highest_rcvd_seq = 0;
    std::int64_t rcvd_seq_register = 0;
    std::int64_t dack_seq_register = 0;
    int      highest_rcvd_ack = 0;
    std::int64_t rcvd_ack_register = 0;
    std::int64_t rcvd_dack_register = 0;
    int      capability     = 0;
    int      num_failures   = 0;
    int      cost           = 0;
    int      energy_level   = 0;
    int      reserve_tslot  = -1;
    int      rtslot_counter = -1;
    int      ackable_tx_count = 0;
    int      ackable_rx_count = 0;
};

struct TslotEntry {
    int  slot     = 0;
    int  node_addr = -1;
    bool reserved = false;
};

class CsrMacCore {
public:
    struct Config {
        int   max_slot_reserve     = 255;   // matches MAX_SLOTRESERVE
        bool  enable_reserve_tslot = true;  // ENABLE_RESERVE_TSLOT
    };

    explicit CsrMacCore(const Config& cfg = Config());

    void   SetNow(double now) { now_ = now; }
    double Now() const        { return now_; }

    // main API we care about today
    int PickTxSlot(int estimated_active_nodes, int rn_range);
    int RenewTxSlot(int current_slot);

    std::vector<NeighborEntry>&       Neighbors()       { return neighbors_; }
    const std::vector<NeighborEntry>& Neighbors() const { return neighbors_; }

private:
    int SlotRangeForNodes(int nodes) const;

    Config cfg_;
    double now_ = 0.0;

    int   txslot_         = -1;
    int   txslot_counter_ = -1;
    int   total_tslot_    = 0;
    double last_tx_time_  = -1.0;

    std::vector<NeighborEntry> neighbors_;
    std::vector<TslotEntry>    tslot_table_;
};
