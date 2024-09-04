import * as J from 'joj-core';
import * as assert from 'ptz-assert';
import * as Ai from './index';

describe('Ai', () => {
    describe('getComputerMove', () => {
        it('Start as black', () => {
            // [[From], ...[to]]
            const allowedMoves = J.Move.getAllowedMovesFromArrays([
                [[0, 7], [0, 6], [1, 6]],
                [[1, 7], [0, 6], [1, 6], [2, 6]],
                [[2, 7], [1, 6], [2, 6], [3, 6]],
                [[3, 7], [2, 6], [3, 6], [4, 6]],
                [[4, 7], [3, 6], [4, 6], [5, 6]],
                [[5, 7], [4, 6], [5, 6], [6, 6]],
                [[6, 7], [5, 6], [6, 6], [7, 6]],
                [[7, 7], [6, 6], [7, 6]]
            ]);

            const game = J.Game.createGame();
            const move = Ai.getComputerMove(game);

            assert.ok(J.Move.movesContains(allowedMoves, move));
        });
    });
});
