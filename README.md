# Tarius AI

Tarius AI is a personal AI twin and secretary that runs locally on your device. It provides a conversational interface for managing tasks, scheduling events, and more, while ensuring your data remains private.

## Features

- **Local Conversation Store**: All conversations are stored locally on your device.
- **Basic Personality Emulation**: The AI responds in a personalized manner.
- **Compression via Summarization**: Older conversations are summarized to save space.
- **AI Secretary Functions**: Schedule events, set reminders, and manage tasks.

## Building from Source

### Prerequisites

- C++17 compatible compiler
- CMake 3.10 or higher
- nlohmann_json library
- spdlog library

### Build Instructions

1. Clone the repository:

   ```
   git clone https://github.com/yourusername/tarius_ai_cpp.git
   cd tarius_ai_cpp
   ```

2. Create a build directory:

   ```
   mkdir build
   cd build
   ```

3. Configure and build:

   ```
   cmake ..
   make
   ```

4. Run the application:
   ```
   ./tarius_ai
   ```

## Usage

Once the application is running, you can interact with Tarius AI through the command-line interface:

- Chat naturally with the AI
- Schedule events (e.g., "Schedule a meeting with Bob tomorrow at 3pm")
- Set reminders (e.g., "Remind me to call mom this evening")
- Ask for help by typing "help"
- Exit the application by typing "exit" or "quit"

## Data Storage

All data is stored locally in the following directories:

- `data/conversations/`: Stored conversations
- `data/summaries/`: Conversation summaries
- `data/calendar/`: Calendar events
- `data/tasks/`: Task list
- `logs/`: Application logs

## License

[MIT License](LICENSE)
