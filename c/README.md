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
