# Shogi Game

A modern Qt6-based Shogi game written in C++17 with a modular architecture.

## Features

### Core Features
- **Local Play**: Human vs AI, AI vs AI, Human vs Human
- **Online Play**: TCP-based multiplayer support
- **Handicap Games (Komaochi)**: Support for traditional handicap setups
  - 香落ち (Lance handicap)
  - 角落ち (Bishop handicap) 
  - 飛車落ち (Rook handicap)
  - 二枚落ち (Rook and Bishop handicap)
  - Custom handicap configurations
- **USI Engine Support**: Integration with external USI-compatible engines
- **Built-in AI**: Minimax-based AI for offline play
- **Game Records**: SFEN format support for game import/export
- **Comprehensive Testing**: Unit tests for core functionality

### GUI Features
- Interactive board display with Japanese piece characters
- Move history tracking
- Game status indicators
- Coordinate display
- Undo functionality

## Architecture

The project follows modern C++ design principles with clear separation of concerns:

```
src/
├── core/          # Core game logic (engine-independent)
│   ├── board/     # Board representation, moves, pieces
│   └── komaochi/  # Handicap game support
├── engine/        # External engine integration
│   └── usi/       # USI protocol implementation
├── ai/            # Built-in AI
│   └── basic/     # Minimax-based AI
├── ui/            # Qt-based user interface
│   └── board/     # Board widget and main window
├── net/           # Network communication
│   └── basic/     # TCP client/server
└── main.cpp       # Application entry point
```

## Build Requirements

- **C++ Compiler**: C++17 compatible (GCC 8+, Clang 7+, MSVC 2019+)
- **Qt6**: Core, Widgets, Network, Test modules
- **CMake**: Version 3.16 or higher

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-tools-dev
```

### macOS (with Homebrew)
```bash
brew install cmake qt6
```

### Windows
Install Qt6 from the official Qt installer or use vcpkg:
```bash
vcpkg install qt6[core,widgets,network]
```

## Building

### Standard Build
```bash
# Clone the repository
git clone <repository-url>
cd shogi

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run tests
ctest --verbose
```

### Development Build with Tests
```bash
# Configure for development
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build everything including tests
cmake --build .

# Run all tests
make run_tests
# Or individually:
./tests/test_piece
./tests/test_board
./tests/test_komaochi
```

## Running

### Basic Usage
```bash
# Run the GUI application
./ShogiGame

# Or from the build directory
./src/ShogiGame
```

### Command Line Options
The application supports standard Qt command line options:
```bash
# Run with specific window size
./ShogiGame -geometry 800x600

# Enable Qt debug output
./ShogiGame -debug
```

## Testing

The project includes comprehensive unit tests for core functionality:

```bash
# Run all tests
ctest

# Run specific test suites
./tests/test_piece     # Piece logic tests
./tests/test_board     # Board and game state tests  
./tests/test_komaochi  # Handicap game tests

# Verbose test output
ctest --verbose
```

### Test Coverage
- **Piece Logic**: Creation, promotion, demotion, equality
- **Board State**: Position setup, move validation, SFEN parsing
- **Handicap Games**: All standard komaochi configurations
- **Move Generation**: Legal move calculation (basic framework)

## Project Structure

### Libraries
- **ShogiCore**: Core game logic and board representation
- **ShogiEngine**: USI engine integration
- **ShogiAI**: Built-in AI implementation
- **ShogiNet**: Network communication layer
- **ShogiUI**: Qt-based graphical interface

### Design Patterns
- **MVC Architecture**: Clear separation between UI and game logic
- **Modern C++**: Smart pointers, RAII, standard containers
- **Non-blocking Design**: Network and AI operations don't block UI
- **Modular Components**: Each feature is a separate library

## Usage Examples

### Basic Game
1. Launch the application: `./ShogiGame`
2. Click "New Game" to start
3. Click on pieces to select and move them
4. Use the "Undo Move" button to take back moves

### Handicap Games
```cpp
// Example: Set up a two-piece handicap game
shogi::core::Board board;
shogi::core::KomaochiManager manager;
manager.applyHandicap(board, shogi::core::KomaochiType::Nimai);
```

### Network Play
```cpp
// Server side
shogi::net::NetworkManager server;
server.startServer(12345);

// Client side  
shogi::net::NetworkManager client;
client.connectToServer("localhost", 12345);
```

### USI Engine Integration
```cpp
// Start external engine
shogi::engine::USIEngine engine;
engine.startEngine("/path/to/usi/engine");
engine.newGame();
engine.go(1000);  // Think for 1 second
```

## Development Roadmap

### Phase 1: Foundation ✅
- [x] CMake project structure
- [x] Core board representation
- [x] Basic piece logic
- [x] Handicap game support
- [x] Unit test framework

### Phase 2: Game Logic (In Progress)
- [ ] Complete legal move generation
- [ ] Check/checkmate detection
- [ ] Game state persistence (SFEN)
- [ ] Move validation

### Phase 3: AI & Engine Integration
- [ ] Enhanced minimax AI
- [ ] USI protocol completion
- [ ] Engine process management
- [ ] AI difficulty settings

### Phase 4: Network & UI Polish
- [ ] Robust network protocol
- [ ] Game lobby system
- [ ] Enhanced UI features
- [ ] Configuration management

## Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Make your changes following the coding standards
4. Add tests for new functionality
5. Ensure all tests pass: `cmake --build . && ctest`
6. Submit a pull request

### Coding Standards
- Use modern C++17 features
- Follow Qt naming conventions for UI code
- Add unit tests for new core functionality
- Document public APIs with Doxygen comments
- Use smart pointers and RAII patterns

## License

This project is open source. See LICENSE file for details.

## Credits

- Built with Qt6 framework
- Follows USI (Universal Shogi Interface) protocol
- Japanese piece characters for authentic display
- Modern C++ architecture patterns