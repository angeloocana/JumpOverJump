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

Install libmemcached:
https://github.com/awesomized/libmemcached?tab=readme-ov-file#from-source

###Compile game:
```bash
   clang ./src/game.c -o ./dist/game -I/usr/local/include -L/usr/local/lib -lmemcached -Wl,-rpath,/usr/local/lib
```

### Run game:
```bash
   ./dist/game
```
