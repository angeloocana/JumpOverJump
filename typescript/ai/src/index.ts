import * as J from 'joj-core';
import { getRandomItem } from 'ptz-math';

/**
 * Get closest positions to win.
 */
function getClosestMovesToWin(piece: J.IPiece): J.IMove[] {
    return piece.whereCanIGo.reduce((closestPositions, position) => {
        const move = { from: piece, to: position };

        if (!closestPositions[0] || closestPositions[0].y > position.y)
            return [move];

        if (closestPositions[0].y === position.y)
            return closestPositions.concat(move);

        return closestPositions;
    }, []);
}

/**
 * Get closest moves to win.
 */
function getClosestMovesToWinForPieces(pieces: J.IPiece[]): J.IMove[] {
    return pieces.reduce((accMoves: J.IMove[], piece) => {
        const moves = getClosestMovesToWin(piece);

        if (!accMoves[0])
            return moves;

        if (!moves[0])
            return accMoves;

        if (moves[0].to.y > accMoves[0].to.y)
            return moves;

        if (moves[0].to.y === accMoves[0].to.y)
            return accMoves.concat(moves);

        return moves;
    }, []);
}

/**
 * Tries to return the best move for a game.
 */
function getComputerMove(game: J.IGame): J.IMove {
    const pieces = J.Game.getTurnPiecesWhereCanIGo(game);

    const moves = getClosestMovesToWinForPieces(pieces);

    return getRandomItem(moves);
}

export {
    getComputerMove
};
