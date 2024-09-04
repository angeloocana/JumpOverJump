import clear from 'clear';
import * as AI from 'joj-ai';
import * as J from 'joj-core';
import readline from 'readline';

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

const getMenu = () =>
    'Welcome to Jump Over Jump! \n\n' +
    'What do you wanna do? \n' +
    '1 - Play vs AI \n' +
    '2 - Play offline \n' +
    '0 - quit \n';

const isAiTurn = (game: J.IGame) => J.Game.getPlayerTurn(game).isAi;

function askForMove(game: J.IGame) {
    console.log(J.Board.printUnicodeBoard(game.board));

    if (isAiTurn(game)) {
        const aiMove = AI.getComputerMove(game);
        return askForMove(J.Move.getGameAfterMove(game, aiMove));
    }

    rl.question('Move (fromXfromYtoXtoY):', (m) => {
        const move = {
            from: { x: Number(m[0]), y: Number(m[1]) },
            to: { x: Number(m[2]), y: Number(m[3]) }
        };

        askForMove(J.Move.getGameAfterMove(game, move));
    });
}

function playAi() {
    const game = J.Game.createGame({
        players: {
            black: { isAi: true, name: 'black' },
            white: { name: 'white' }
        }
    });
    askForMove(game);
}

function playOffline() {
    const game = J.Game.createGame();
    askForMove(game);
}

function openMenu() {
    clear();

    rl.question(getMenu(), (answer) => {
        switch (answer) {
            case '1': return playAi();
            case '2': return playOffline();
            case '0': return rl.close();
            default: console.log('invalid answer!'); openMenu();
        }
    });
}

openMenu();
