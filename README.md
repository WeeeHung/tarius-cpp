# Tarius AI - Personal AI Assistant

Tarius is a locally-run AI assistant that combines scheduling, reminders, and conversational abilities.

## Features

- 💬 Chat naturally with your AI twin
- 📅 Schedule events and appointments
- ⏰ Set reminders and manage tasks
- 📝 Conversation memory and retrieval
- 🔒 100% private - all data stays local on your device
- 🤖 Optional local LLM integration (no internet required)

## Building from Source

### Prerequisites

- CMake (3.10 or higher)
- C++17 compatible compiler
- Git

### Build Instructions

1. Clone the repository with submodules:

   ```
   git clone --recursive https://github.com/yourusername/tarius-cpp.git
   cd tarius-cpp
   ```

2. If you cloned without `--recursive`, initialize and update submodules:

   ```
   git submodule update --init --recursive
   ```

3. Create a build directory and configure:

   ```
   mkdir build
   cd build
   cmake ..
   ```

4. Build the project:
   ```
   cmake --build . --config Release
   ```

## Using with a Local LLM

Tarius can use a local LLaMA model for generating responses:

1. Download a compatible GGUF model (recommended size: 1-7B parameters)

   - Example: `curl -L -o models/Dolphin3.0-Llama3.2-1B-Q4_K_M.gguf https://huggingface.co/bartowski/Dolphin3.0-Llama3.2-1B-GGUF/resolve/main/Dolphin3.0-Llama3.2-1B-Q4_K_M.gguf`

2. Run Tarius:

   ```
   ./build/tarius_ai
   ```

3. Load the model within the application:

   ```
   You: /load_model ../models/Dolphin3.0-Llama3.2-1B-Q4_K_M.gguf
   ```

4. Check model status:
   ```
   You: /model_status
   ```

## Available Commands

- `help` - Display help message
- `exit` or `quit` - Exit the application
- `/load_model [path]` - Load a GGUF model file
- `/model_status` - Check if the LLaMA model is active

## Example Usage

```
You: Schedule a meeting with Bob tomorrow at 3pm
Tarius: I've scheduled "meeting with Bob" for April 23, 2024 at 3:00 PM. I'll remind you when it's time.

You: Remind me to call mom this evening
Tarius: I'll remind you to "call mom" on April 22, 2024 at 7:00 PM.

You: Tell me a joke
Tarius: Why don't scientists trust atoms? Because they make up everything!
```

## License

MIT License - See LICENSE file for details.
