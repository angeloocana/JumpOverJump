# C implementation of the Jump Over Jump game
Play online game at [jumpoverjump.com](https://jumpoverjump.com/).

## Play on terminal

### Dependencies

#### Mac OS

Install CMake:
```bash
   brew install cmake
```

Install memcached:
```bash
   brew install memcached
```

Change memcached item memory limit to 2bm (default is 64mb):
Open `/opt/homebrew/opt/memcached/homebrew.mxcl.memcached.plist`
Add this to ProgramArguments:
```bash
   <string>-m</string>
   <string>2</string>
```

Start memcached:
```bash
   brew services start memcached
```

Install libmemcached:
https://github.com/awesomized/libmemcached?tab=readme-ov-file#from-source

Install pcg:
https://www.pcg-random.org/using-pcg-c.html

###Compile game:
```bash
   make
```

### Run game:
```bash
   ./dist/game
```

## TODO

Optimize the code:
- Use bitwise operations to store the board
- Use a more efficient data structure to store the board history in a local cache
- Use a more efficient algorithm to find the possible moves
- Use a more efficient algorithm to find the winner
- Use static inline functions where possible
- Use const variables where possible
- Add multithreading where possible
- Save the games in a queue and create a job to write them to the disk
- Fetch memcached keys in batches

AI:
- Build neural network to predict the best move
- AlphaZero Monte Carlo Tree Search + Neural Network

WebAssembly:
- Build a version of the game using emscripten to run on the web

