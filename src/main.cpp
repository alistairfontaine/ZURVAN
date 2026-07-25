#include "zurvan.hpp"
#include "shell.hpp"
#include <iostream>

int main() {
    std::cout << "🧬 ======================================================== 🧬" << std::endl;
    std::cout << "🔥 ZURVAN: Decentralized High-Frequency Clock Tuning Shell 🔥" << std::endl;
    std::cout << "🧬 ======================================================== 🧬" << std::endl;

    Zurvan::SynchronizationEngine engine;

    // Launch the interactive interpretation command loop on bare metal
    Zurvan::launch_interactive_shell(engine);

    return 0;
}
