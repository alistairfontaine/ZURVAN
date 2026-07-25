#include "zurvan.hpp"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>

namespace Zurvan {

SynchronizationEngine::SynchronizationEngine() {}

SynchronizationEngine::~SynchronizationEngine() {}

/**
 * HIGH-RESOLUTION MICROSECOND CLOCK HOOK
 * Grabs steady monotonic system clock ticks since startup to establish our absolute timing baseline.
 */
uint64_t SynchronizationEngine::get_local_microseconds() const {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}

/**
 * RAM SNAPSHOT REGISTRATION PRIMITIVE
 * Bundles network latency ticks and clock metrics into a packed structural layout frame.
 */
bool SynchronizationEngine::log_snapshot_to_cache(uint64_t remote_ts, uint32_t rtt, float calculated_offset, uint8_t sync_lock) {
    ClockSnapshot snapshot;
    snapshot.local_timestamp_us = get_local_microseconds();
    snapshot.remote_timestamp_us = remote_ts;
    snapshot.round_trip_time_us = rtt;
    snapshot.estimated_drift_us = calculated_offset;
    snapshot.is_synchronized = sync_lock;

    sync_history_cache_.push_back(snapshot);
    return true;
}

/**
 * 📦 .ZVN CONTAINER SERIALIZATION PIPELINE 📦
 * Commits the active volatile RAM history logging cache directly to an independent binary file.
 */
bool SynchronizationEngine::serialize_history_to_disk(const std::string& host_path) {
    if (sync_history_cache_.empty()) return false;

    std::ofstream out_file(host_path, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!out_file.is_open()) {
        std::cerr << "❌ Data Write Exception: Failed to secure file allocation descriptor locks on: " << host_path << std::endl;
        return false;
    }

    // Write raw vector array memory blocks straight to disk sectors, bypassing high-level library abstractions
    out_file.write(reinterpret_cast<const char*>(sync_history_cache_.data()), sync_history_cache_.size() * sizeof(ClockSnapshot));
    out_file.flush();
    out_file.close();

    std::cout << "✓ [.zvn Serialized] Timing history logged to disk. Saved "
              << sync_history_cache_.size() << " tracking records." << std::endl;
    return true;
}

/**
 * 📡 .ZVN FILE INGEST DESERIALIZER 📡
 * Streams a raw .zvn container off the partition to completely reconstruct the timing history cache.
 */
bool SynchronizationEngine::deserialize_history_from_disk(const std::string& host_path) {
    std::ifstream in_file(host_path, std::ios::binary | std::ios::ate);
    if (!in_file.is_open()) {
        std::cerr << "❌ Ingest Exception: Could not trace target telemetry database file: " << host_path << std::endl;
        return false;
    }

    std::streamsize file_size = in_file.tellg();
    in_file.seekg(0, std::ios::beg);

    size_t total_records_detected = file_size / sizeof(ClockSnapshot);
    if (total_records_detected == 0) {
        in_file.close();
        return false;
    }

    sync_history_cache_.resize(total_records_detected);
    in_file.read(reinterpret_cast<char*>(sync_history_cache_.data()), file_size);
    in_file.close();

    std::cout << "✓ [.zvn Ingested] Memory timeline index successfully rebuilt. Caching "
              << total_records_detected << " historical drift frames." << std::endl;
    return true;
}

/**
 * 🖨️ DETAILED TIMING REPORT MATRIX PRINTER 🖨️
 */
void SynchronizationEngine::display_sync_history() const {
    std::cout << "\n⏳ [ZURVAN Telemetry Ledger] Historical Synchronization Log:" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << "LOCAL_US\t\tREMOTE_US\t\tRTT_US\tOFFSET_US\tSTATUS" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;

    if (sync_history_cache_.empty()) {
        std::cout << "[Empty timing telemetry log cache]" << std::endl;
        return;
    }

    for (const auto& snapshot : sync_history_cache_) {
        std::string status_str = snapshot.is_synchronized ? "LOCKED" : "DRIFTING";
        std::cout << snapshot.local_timestamp_us << "\t"
                  << snapshot.remote_timestamp_us << "\t"
                  << snapshot.round_trip_time_us << "\t"
                  << snapshot.estimated_drift_us << "\t\t"
                  << status_str << std::endl;
    }
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
}

} // namespace Zurvan
