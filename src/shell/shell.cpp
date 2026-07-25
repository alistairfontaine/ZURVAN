#include "shell.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

namespace Zurvan {

void launch_interactive_shell(SynchronizationEngine& engine) {
    std::string line;
    std::cout << "\n🛡️ [ZURVAN Clock Synchronization Engine Prompt Active] Type 'help' to review command matrices." << std::endl;

    while (true) {
        std::cout << "zurvan-cli> ";
        if (!std::getline(std::cin, line)) break;

        if (line.empty()) continue;

        // Tokenize input string fragments cleanly using standard string stream arrays
        std::stringstream ss(line);
        std::string command;
        ss >> command;

        std::vector<std::string> args;
        std::string arg;
        while (ss >> arg) {
            args.push_back(arg);
        }

        // --- COMMAND ROUTING INDEX MATRIX ---
        if (command == "exit" || command == "quit") {
            std::cout << "👋 Deactivating synchronization interfaces. Timing clock offline." << std::endl;
            break;
        }
        else if (command == "help") {
            std::cout << "\n📋 Operational ZURVAN Core Primitives:" << std::endl;
            std::cout << "  sample <offset_us> <variance> - Ingests a raw network offset handshake and updates the Kalman filter" << std::endl;
            std::cout << "  regression                    - Computes the least-squares linear trend line slope across logs" << std::endl;
            std::cout << "  predict <duration_us>         - Forecasts the future desynchronization clock offset boundary" << std::endl;
            std::cout << "  view                          - Prints out a detailed ledger list view of all cached snapshots" << std::endl;
            std::cout << "  save <file.zvn>               - Serializes the active timeline history data directly to disk" << std::endl;
            std::cout << "  load <file.zvn>               - Ingests a raw .zvn container file and rebuilds table caches" << std::endl;
            std::cout << "  exit / quit                   - Safely releases descriptors and terminates the shell environment\n" << std::endl;
        }
        else if (command == "sample") {
            if (args.size() < 2) {
                std::cerr << "⚠️ Usage error: sample <measured_offset_us> <network_jitter_variance>" << std::endl;
                continue;
            }
            float measured_offset = std::stof(args[0]);
            float jitter_variance = std::stof(args[1]);

            engine.update_kalman_filter(measured_offset, jitter_variance);
            std::cout << "✓ Kalman filter updated. Filtered Offset Estimate: " << engine.get_estimated_offset() << " us" << std::endl;
        }
        else if (command == "regression") {
            engine.calculate_linear_drift_rate();
            std::cout << "📊 Calculated Internal Clock Drift Acceleration Profile:" << std::endl;
            std::cout << "  Drift Rate Velocity Coefficient: " << engine.get_estimated_drift_rate() << " us per second." << std::endl;
        }
        else if (command == "predict") {
            if (args.empty()) {
                std::cerr << "⚠️ Usage error: predict <future_duration_microseconds>" << std::endl;
                continue;
            }
            uint64_t duration_us = std::stoull(args[0]);
            float projection = engine.predict_future_offset(duration_us);
            std::cout << "🔮 Predicted Clock Desynchronization Offset Position at Horizon: " << projection << " us" << std::endl;
        }
        else if (command == "view") {
            engine.display_sync_history();
        }
        else if (command == "save") {
            if (args.empty()) {
                std::cerr << "⚠️ Usage error: save <filename.zvn>" << std::endl;
                continue;
            }
            std::string dest_path = "tests/" + args[0]; // Isolate binary outputs straight to tests/ directory
            engine.serialize_history_to_disk(dest_path);
        }
        else if (command == "load") {
            if (args.empty()) {
                std::cerr << "⚠️ Usage error: load <filename.zvn>" << std::endl;
                continue;
            }
            std::string source_path = "tests/" + args[0];
            engine.deserialize_history_from_disk(source_path);
        }
        else {
            std::cerr << "❌ Unknown primitive: Type 'help' to review structural command arrays." << std::endl;
        }
    }
}

} // namespace Zurvan
