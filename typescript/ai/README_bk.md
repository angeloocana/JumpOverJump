joj-ai
================
[![Build Status](https://travis-ci.org/angeloocana/joj-ai.svg)](https://travis-ci.org/angeloocana/joj-ai)
[![NPM](https://img.shields.io/npm/v/joj-ai.svg)](https://www.npmjs.com/package/joj-ai)
[![codecov.io](http://codecov.io/github/angeloocana/joj-ai/coverage.svg)](http://codecov.io/github/angeloocana/joj-ai)
[![Dependency Status](https://gemnasium.com/angeloocana/joj-ai.svg)](https://gemnasium.com/angeloocana/joj-ai)
[![bitHound Score](https://www.bithound.io/github/gotwarlost/istanbul/badges/score.svg)](https://www.bithound.io/github/angeloocana/joj-ai)
![License](https://img.shields.io/npm/l/joj-ai.svg)

> JumpOverJump.com game AI modules
> 
> JumpOverJump is an opensource AI game board.
> Be welcome to learn, help and play!!!

## Install

Install with npm:

```bash
    $ npm install joj-ai --save
```

or clone from github:

```bash
    $ git clone https://github.com/angeloocana/joj-ai.git
```

## Setup

```bash
    $ npm install 
    $ typings install
```

*Add to your typings/index.d.ts

```ts
/// <reference path="../node_modules/joj-ai/src/typings/index.d.ts" />
```

## Test

```bash
    $ npm test
``` 

## Usage

### New Game - AI vs Human

```js

import {Game, Players, Player, AiMedium} from 'joj-ai';
import Ai from 'joj-ai';

let players = new Players({
    white: new Player({
        name: "Computer",
        ai: new Ai()
    }),
    black: new Player({ name: "Gabi" })
});

let game = new Game({
    players
});

game.move({ x: 2, y: 7 }, { x: 2, y: 6 });
//Computer plays automatic after a move
game.move({ x: 2, y: 0 }, { x: 2, y: 1 });

```
