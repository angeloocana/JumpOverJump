"use strict";

angular.module("jojApp").controller("jojComputerVsComputerController", [
    "jojGameService", "$timeout", "jojPlayerService", "jojAIService", "$log", "jojGameStorageService",
    function (jojGameService, $timeout, jojPlayerService, jojAIService, $log, jojGameStorageService) {

        var players = jojPlayerService.getPlayersVsComputer(true);
        var game;
        var computerMove;

        var getComputerMove = function () {
            while (!game.ended) {
                computerMove = jojAIService.getComputerMove(game);
                if (computerMove) {
                    $log.info("Moving");
                    game = jojGameService.move(game, computerMove.startPosition, computerMove.nextPosition);
                } else {
                    $log.info("No Move");
                    $log.info(game);
                }

                if (!game.computerTurn || (game.players.black.isComputer && !computerMove)) {
                    game.computerTurn = true;
                    game.players.computerIsWhite = !game.players.computerIsWhite;
                    game.players.black.isComputer = !game.players.computerIsWhite;
                    game.players.white.isComputer = game.players.computerIsWhite;
                }
            }
        }

        var play = function () {
            $log.info("New Game");
            game = jojGameService.newGame(null, players);
            
            getComputerMove();

            jojGameStorageService.saveGame(game);

            if (computerMove)
                $timeout(play, 500);
        }

        $timeout(play, 500);
    }]);
