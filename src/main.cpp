#include "zurvan.hpp"
#include <iostream>
#include <vector>
#include <unistd.h>

int main() {
    std::cout << "🧬 ======================================================== 🧬" << std::endl;
    std::cout << "🔥 ZURVAN: Decentralized High-Frequency Clock Tuning Core  🔥" << std::endl;
    std::cout << "🧬 ======================================================== 🧬" << std::endl;

    Zurvan::SynchronizationEngine engine;
    std::string test_storage_vault = "tests/sync_telemetry.zvn";

    std::cout << "📝 [Jitter Simulation] Feeding noisy, drifting network timestamps..." << std::endl;

    // Simulated sequence of raw clock offsets (in microseconds) containing severe network latency noise
    std::vector<float> simulated_jittery_offsets = {-15.4f, -14.2f, -19.8f, -18.1f, -24.4f, -22.9f};
    float network_jitter_variance_R = 4.5f; // Configure expected measurement noise parameter variance (R)

    for (size_t i = 0; i < simulated_jittery_offsets.size(); ++i) {
        float raw_input_offset = simulated_jittery_offsets[i];

        // 🔒 Fire the live scalar Kalman filter prediction and correction loops on bare metal
        engine.update_kalman_filter(raw_input_offset, network_jitter_variance_R);

        std::cout << "  Sample [" << i << "] Raw Input Offset: " << raw_input_offset
                  << " us | Filtered Kalman State Estimate: " << engine.get_estimated_offset() << " us" << std::endl;

        // Sleep the thread briefly to allow monotonic clock time intervals to advance naturally
        usleep(50000);
    }

    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << "🔍 [Regression Analysis] Extracting constant crystal clock drift velocity..." << std::endl;

    // Compute the least-squares linear trend line slope across the newly recorded history metrics
    engine.calculate_linear_drift_rate();
    std::cout << "  Calculated Clock Drift Rate Velocity: " << engine.get_estimated_drift_rate() << " us per second." << std::endl;

    std::cout << "--------------------------------------------------------------------------------" << std::endl;

    // ⏳ Forecast the future clock position exactly 15 seconds (15,000,000 microseconds) into the future
    uint64_t future_horizon_us = 15000000;
    std::cout << "⏳ [Temporal Prediction] Forecasting clock drift state 15s into the future..." << std::endl;
    float projected_drift_offset = engine.predict_future_offset(future_horizon_us);
    std::cout << "  Predicted Clock Desynchronization Position at Horizon: " << projected_drift_offset << " us" << std::endl;

    // 3. Serialize our final, stabilized timeline trends to our custom .zvn container standard
    engine.serialize_history_to_disk(test_storage_vault);

    std::cout << "\n🚀 [Status Update] Milestone 2 scalar Kalman filters and drift trend loops functional!" << std::endl;
    std::cout << "🧬 ======================================================== 🧬" << std::endl;
    return 0;
}
