#include "zurvan.hpp"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
#include <cmath>

namespace Zurvan {

SynchronizationEngine::SynchronizationEngine()
    : estimated_offset_us_(0.0f),
      estimation_error_cov_(1.0f),      // Initial uncertainty bound constant
      process_noise_q_(0.001f),         // Small process noise parameter tracking drift shifts
      estimated_drift_rate_(0.0f),      // Baseline initialization drift velocity
      last_update_time_us_(0) {}

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
bool Zurvan::SynchronizationEngine::serialize_history_to_disk(const std::string& host_path) {
    if (sync_history_cache_.empty()) return false;

    std::ofstream out_file(host_path, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!out_file.is_open()) {
        std::cerr << "❌ Data Write Exception: Failed to secure file allocation descriptor locks on: " << host_path << std::endl;
        return false;
    }

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
bool Zurvan::SynchronizationEngine::deserialize_history_from_disk(const std::string& host_path) {
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
void Zurvan::SynchronizationEngine::display_sync_history() const {
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

/**
 * 🧠 BARE-METAL SCALAR KALMAN FILTER ITERATION STEP 🧠
 * Executes real-time prediction and measurement correction to eliminate network jitter noise.
 */
void SynchronizationEngine::update_kalman_filter(float measured_offset_us, float network_jitter_variance) {
    uint64_t current_time_us = get_local_microseconds();

    if (last_update_time_us_ == 0) {
        last_update_time_us_ = current_time_us;
        estimated_offset_us_ = measured_offset_us;
        return;
    }

    // Compute delta time interval scaled into standard fractional seconds
    float delta_t_sec = static_cast<float>(current_time_us - last_update_time_us_) / 1000000.0f;
    last_update_time_us_ = current_time_us;

    // 1. Prediction Pass (Project state variables forward using drift velocity trend lines)
    float predicted_offset = estimated_offset_us_ + (estimated_drift_rate_ * delta_t_sec);
    estimation_error_cov_ = estimation_error_cov_ + process_noise_q_;

    // 2. Correction Pass (Compute the optimal Kalman gain ratio to weigh measurement reliability)
    float kalman_gain = estimation_error_cov_ / (estimation_error_cov_ + network_jitter_variance);

    // Adjust true offset estimate and lock the updated covariance parameters
    estimated_offset_us_ = predicted_offset + kalman_gain * (measured_offset_us - predicted_offset);
    estimation_error_cov_ = (1.0f - kalman_gain) * estimation_error_cov_;

    // Log the resulting locked telemetry directly to the history table cache
    uint8_t standard_phase_lock = (std::abs(estimated_offset_us_) < 1.0f) ? 1 : 0;
    log_snapshot_to_cache(current_time_us + static_cast<uint64_t>(estimated_offset_us_), 200, estimated_offset_us_, standard_phase_lock);
}

/**
 * 📈 LEAST-SQUARES LINEAR REGRESSION CORE 📈
 * Ingests cached historical data frames to establish the true drift velocity rate of the local crystal.
 */
void SynchronizationEngine::calculate_linear_drift_rate() {
    if (sync_history_cache_.size() < 2) return;

    double sum_t = 0.0, sum_offset = 0.0, sum_t_squared = 0.0, sum_t_offset = 0.0;
    size_t N = sync_history_cache_.size();

    uint64_t base_time = sync_history_cache_[0].local_timestamp_us;

    for (const auto& snapshot : sync_history_cache_) {
        double current_t_sec = static_cast<double>(snapshot.local_timestamp_us - base_time) / 1000000.0;
        double current_offset = static_cast<double>(snapshot.estimated_drift_us);

        sum_t += current_t_sec;
        sum_offset += current_offset;
        sum_t_squared += current_t_sec * current_t_sec;
        sum_t_offset += current_t_sec * current_offset;
    }

    // Standard linear regression slope divisor math loop calculation
    double denominator = (N * sum_t_squared) - (sum_t * sum_t);
    if (std::abs(denominator) < 1e-8) return;

    double calculated_slope = ((N * sum_t_offset) - (sum_t * sum_offset)) / denominator;
    estimated_drift_rate_ = static_cast<float>(calculated_slope); // Velocity scale locked (us/sec)
}

/**
 * 🔮 FORECASTING PREDICTION MATRIX PRIMITIVE 🔮
 * Evaluates the calculated drift rate slope trends to project accurate desynchronization timelines.
 */
float SynchronizationEngine::predict_future_offset(uint64_t future_duration_us) const {
    float future_dt_sec = static_cast<float>(future_duration_us) / 1000000.0f;
    return estimated_offset_us_ + (estimated_drift_rate_ * future_dt_sec);
}

} // namespace Zurvan
