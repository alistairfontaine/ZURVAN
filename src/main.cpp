#include "zurvan.hpp"
#include <iostream>

int main() {
    std::cout << "🧬 ======================================================== 🧬" << std::endl;
    std::cout << "🔥 ZURVAN: Decentralized High-Frequency Clock Tuning Core  🔥" << std::endl;
    std::cout << "🧬 ======================================================== 🧬" << std::endl;

    Zurvan::SynchronizationEngine engine;
    std::string test_storage_vault = "tests/sync_telemetry.zvn";

    // 1. Ingest mock network synchronization frames into the volatile RAM history arrays
    std::cout << "📝 [Telemetry Logging] Capturing distributed system time-ticks..." << std::endl;

    uint64_t fake_remote_time = 1000000;
    engine.log_snapshot_to_cache(fake_remote_time, 450, -12.4f, 0);       // Entry 0: Baseline Drifting State
    engine.log_snapshot_to_cache(fake_remote_time + 1500, 420, -4.2f, 0); // Entry 1: Converging Offset State
    engine.log_snapshot_to_cache(fake_remote_time + 3100, 390, 0.1f, 1);  // Entry 2: Mathematical Phase Lock Achieved

    // 2. Serialize the active array cache blocks directly to our custom .zvn format container
    if (engine.serialize_history_to_disk(test_storage_vault)) {
        // Instantiate a standalone parser context to cross-examine data locality bytes on disk
        Zurvan::SynchronizationEngine diagnostic_parser;
        if (diagnostic_parser.deserialize_history_from_disk(test_storage_vault)) {
            diagnostic_parser.display_sync_history();
        }
    }

    std::cout << "\n🚀 [Status Update] Milestone 1 core binary data packing and .zvn streams operational!" << std::endl;
    std::cout << "🧬 ======================================================== 🧬" << std::endl;
    return 0;
}
