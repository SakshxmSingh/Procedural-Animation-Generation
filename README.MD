## How to run the code

1. Clone the repository
2. Open the terminal and navigate to the project folder
3. Install the following packages:
   - `cmake`
   - `make`
   - `gcc`
   - `g++`
   - `clang`
   - `sfml`
- For Ubuntu/WSL, you can install the packages using the following command:
  ```bash
  sudo apt-get install cmake make gcc g++ clang libsfml-dev
  ```
- For MacOS, you can install the packages using the following command:
  ```bash
  brew install cmake make gcc clang sfml
  ```
4. Run the following commands:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ./MyGame
   ```
5. Enjoy the game!