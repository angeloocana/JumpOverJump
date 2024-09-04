"use strict";

angular.module("jojApp").factory("jojGameStorageService",
    ["$log", "$localStorage", "jojGameService", "$http", "$q", "urlServer",
        function ($log, $localStorage, jojGameService, $http, $q, urlServer) {

            var saveGame = function (game) {
                var cleanGame = {
                    ended: game.ended,
                    movements: [],
                    blackWin: game.blackWin
                };

                for (var i = 0; i < game.movements.length; i++) {
                    var move = game.movements[i];
                    cleanGame.movements.push({
                        startPosition: { x: move.startPosition.x, y: move.startPosition.y },
                        nextPosition: { x: move.nextPosition.x, y: move.nextPosition.y }
                    });
                }

                var deferred = $q.defer();
                $http({ method: "POST", url: urlServer("api/game/Save"), data: cleanGame }).
                    success(function (data, status, headers, config) {
                        $log.info("success");
                        deferred.resolve(data);
                    }).
                    error(function (data, status, headers, config) {
                        if ($localStorage.gameNotSaved)
                            $localStorage.gameNotSaved = [];

                        $localStorage.gameNotSaved.push(game);

                        $log.info("error");
                        deferred.reject(status);
                    });

                return deferred.promise;
            }

            var getGame = function (boardOptions) {
                var game = $localStorage.game;
                if (!game) {
                    game = jojGameService.getCleanGame(game, boardOptions);
                    $localStorage.game = game;
                } else if (boardOptions) {
                    game.boardOptions = boardOptions;
                    $localStorage.game = game;
                }

                return $localStorage.game;
            }

            var setGame = function (game) {
                $localStorage.game = game;

                if (game.ended)
                    saveGame(game);

                if ($localStorage.gameNotSaved && $localStorage.gameNotSaved.length > 0) {
                    var gameNotSaved = $localStorage.gameNotSaved.pop();
                    while (gameNotSaved){
                        saveGame(gameNotSaved);
                        gameNotSaved = $localStorage.gameNotSaved.pop();
                    }
                }

                return getGame();
            }

            var factoryReset = function () {
                $localStorage.game = null;
            }

            return {
                getGame: getGame,
                setGame: setGame,
                factoryReset: factoryReset,
                saveGame: saveGame
            };
        }]);