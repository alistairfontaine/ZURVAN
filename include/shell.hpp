#ifndef SHELL_HPP
#define SHELL_HPP

#include "zurvan.hpp"

namespace Zurvan {

/**
 * 💻 THE OFFICIAL INTERACTIVE TERMINAL SHELL LOOP 💻
 * Hooks directly into standard input/output streams to spin an ultra-minimalist CLI workspace.
 * Parses user command strings and maps inputs straight onto low-level synchronization methods.
 */
void launch_interactive_shell(SynchronizationEngine& engine);

} // namespace Zurvan

#endif // SHELL_HPP
