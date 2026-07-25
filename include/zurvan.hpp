#ifndef ZURVAN_HPP
#define ZURVAN_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace Zurvan {

#pragma pack(push, 1) // Enforce hard structural padding constraints for rapid binary block writes
/**
 * Packed Clock Telemetry Frame
 * Size: Exactly 25 bytes per record, optimizing cache locality and preventing stack fragmentation.
 */
struct ClockSnapshot {
    uint64_t local_timestamp_us;  // 64-bit microsecond monotonic time of the local machine
    uint64_t remote_timestamp_us; // 64-bit reported clock timestamp of the network peer
    uint32_t round_trip_time_us;  // 32-bit calculated round-trip latency network delay
    float    estimated_drift_us;  // 32-bit floating point clock offset desynchronization metric
    uint8_t  is_synchronized;     // 8-bit boolean operational lock indicator flag (0=No, 1=Locked)
};
#pragma pack(pop)

class SynchronizationEngine {
public:
    SynchronizationEngine();
    ~SynchronizationEngine();

    // Milestone 1 Core Primitives
    bool log_snapshot_to_cache(uint64_t remote_ts, uint32_t rtt, float calculated_offset, uint8_t sync_lock);
    bool serialize_history_to_disk(const std::string& host_path);
    bool deserialize_history_from_disk(const std::string& host_path);
    void display_sync_history() const;

    // 🚀 Milestone 2 Core Primitives (Bare-Metal Kalman Drift Correction Filter)
    void update_kalman_filter(float measured_offset_us, float network_jitter_variance);
    void calculate_linear_drift_rate();
    float predict_future_offset(uint64_t future_duration_us) const;

    // State Tracking Getters
    size_t get_cached_snapshot_count() const { return sync_history_cache_.size(); }
    uint64_t get_local_microseconds() const;
    float get_estimated_offset() const { return estimated_offset_us_; }
    float get_estimated_drift_rate() const { return estimated_drift_rate_; }

private:
    std::vector<ClockSnapshot> sync_history_cache_; // High-speed RAM timeline logging array cache

    // 🔒 SCALAR KALMAN FILTER STATE VARIABLES
    float estimated_offset_us_;    // Current estimated true offset (x)
    float estimation_error_cov_;    // State estimation error covariance (P)
    float process_noise_q_;         // Environmental system process noise (Q)
    float estimated_drift_rate_;   // Microseconds per second clock drift velocity (r)
    uint64_t last_update_time_us_;  // Microsecond timestamp marker tracking the last pass (t_prev)
};


} // namespace Zurvan

#endif // ZURVAN_HPP
