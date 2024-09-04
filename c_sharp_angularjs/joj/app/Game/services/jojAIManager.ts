"use strict";

angular.module("jojApp").factory("jojAIManager",
    ["$http", "$q", "jojAIService", "urlServer", "$log",
        function ($http, $q, jojAIService, urlServer, $log) {

            var getServerBestMove = function (game) {
                var deferred = $q.defer();

                var gameInProgress = { isBlackTurn: !game.whiteTurn, Pieces: [] };

                for (var i = 0; i < game.white.pieces.length; i++) {
                    gameInProgress.Pieces.push({
                        isBlack: false,
                        x: game.white.pieces[i].x,
                        y: game.white.pieces[i].y
                    });
                }

                for (var i = 0; i < game.black.pieces.length; i++) {
                    gameInProgress.Pieces.push({
                        isBlack: true,
                        x: game.black.pieces[i].x,
                        y: game.black.pieces[i].y
                    });
                }

                $log.info("gameInProgress");
                $log.info(gameInProgress);

                $http({ method: "POST", url: urlServer("api/game/BestMove"), data: gameInProgress }).
                    success(function (data, status, headers, config) {
                        $log.info("success");
                        $log.info(data);
                        deferred.resolve(data);
                    }).
                    error(function (data, status, headers, config) {
                        $log.info("error");
                        deferred.reject(data);
                    });

                return deferred.promise;
            }

            var getBestMoveFromServer = function (looking, wherePiecesCanGo) {
                var bestMove = null;

                for (var i = 0; i < looking.length; i++) {
                    if (!bestMove || (bestMove.nMovesToFinish > looking[i].nMovesToFinish && looking[i].nMovesToFinish > 0)) {

                        var move = looking[i].move;

                        for (var j = 0; j < wherePiecesCanGo.length; j++) {
                            var piece = wherePiecesCanGo[j];
                            if (piece.x === move.startPosition.x && piece.y === move.startPosition.y) {
                                for (var k = 0; k < piece.whereCanIGo.positions.length; k++) {
                                    var whereCanIGo = piece.whereCanIGo.positions[k];
                                    if (whereCanIGo.x === move.nextPosition.x && whereCanIGo.y === move.nextPosition.y)
                                        bestMove = looking[i];
                                }
                            }
                        }
                    }
                }

                return bestMove;
            }

            var getComputerMove = function (game) {
                $log.info("game");
                $log.info(game);

                var deferred = $q.defer();

                if (game.ended)
                    deferred.resolve(null);
                else if (!game.players.vsComputer)
                    deferred.resolve(null);
                else if (game.players.computerIsWhite !== game.whiteTurn)
                    deferred.resolve(null);
                else {
                    var localBestMove = jojAIService.getBestMove(game);

                    getServerBestMove(game)
                        .then(function (data) {
                            var wherePiecesCanGo = jojAIService.fillPiecesWhereCanTheyGo(game);

                            var serverBestMove = getBestMoveFromServer(data.lookingBlack, wherePiecesCanGo);

                            if (!serverBestMove) {
                                serverBestMove = getBestMoveFromServer(data.lookingWhite, wherePiecesCanGo);
                                if (serverBestMove)
                                    $log.info("Using lookingWhite");
                            }
                            else
                                $log.info("Using lookingBlack");

                            if (serverBestMove) {
                                deferred.resolve(serverBestMove.move);
                            }
                            else {
                                $log.info("Using local");
                                deferred.resolve(localBestMove);
                            }
                        });
                }

                return deferred.promise;
            }

            return {
                getComputerMove: getComputerMove
            };
        }]);