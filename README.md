# Shogi Game

A comprehensive Japanese Chess (Shogi) game implemented in C++23 with wxWidgets.

## Features

### Phase 1 (Implemented)
- ✅ GUI framework with wxWidgets
- ✅ Board and piece rendering
- ✅ Complete Shogi game logic and rules
- ✅ Drag & drop and click & click piece movement
- ✅ Local multiplayer (human vs human)
- ✅ Game state validation (check, checkmate, stalemate)
- ✅ Hand management (captured pieces)

### Planned Features
- **Phase 2**: AI Integration via USI protocol
- **Phase 3**: Online multiplayer with P2P networking
- **Phase 4**: Handicap games and game record formats

## Requirements

- C++23 compatible compiler (GCC 13+, Clang 16+, or MSVC 19.35+)
- wxWidgets 3.2 or later
- CMake 3.25 or later

## Building

### Ubuntu/Debian

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential cmake pkg-config
sudo apt install libwxgtk3.2-dev

# Clone and build
git clone <repository-url>
cd shogi
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Windows (MSYS2)

```bash
# Install MSYS2, then in MSYS2 terminal:
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-wxwidgets3.2

# Clone and build
git clone <repository-url>
cd shogi
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
make -j$(nproc)
```

### macOS

```bash
# Install dependencies (using Homebrew)
brew install cmake wxwidgets

# Clone and build
git clone <repository-url>
cd shogi
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

## Running

After building, run the executable:

```bash
# From build directory
./shogi
```

## Game Rules

This implementation follows standard Shogi rules:

- 9x9 board with traditional piece movement
- Pieces can be captured and dropped back into play
- Promotion zones (last 3 rows for each player)
- Special rules: nifu (二歩), uchifuzume (打ち歩詰め)
- Win conditions: checkmate, resignation

## Controls

- **Click & Click**: Click a piece to select, then click destination
- **Drag & Drop**: Drag pieces from source to destination
- **Promotion**: Automatic when entering/within promotion zone
- **Menu**: Access game options via the menu bar

## Technical Details

### Architecture

- **Game Logic**: Separated from GUI for clean architecture
- **Board State**: Immutable state management
- **Move Validation**: Complete rule checking including edge cases
- **Rendering**: SVG-based graphics with fallback text rendering

### C++23 Features Used

- `std::expected` for error handling
- `std::format` for string formatting
- `std::ranges` for algorithms
- Smart pointers throughout
- Modern enum classes and constexpr

## File Structure

```
├── src/           # Source files
├── include/       # Header files
├── assets/        # Game assets (SVG files)
├── CMakeLists.txt # Build configuration
└── README.md      # This file
```

## Contributing

1. Follow the existing code style
2. Use C++23 features where appropriate
3. Maintain separation between game logic and GUI
4. Add tests for new functionality

## License

[License information to be added]