"use strict";

angular.module("jojApp").directive("jojBoard",
    ["jojGameService", "jojGameStorageService", "jojAIManager", "$window", "$localStorage", "$ionicLoading", "$rootScope", "$log", "$timeout", "$state", "ptzAds",
        function (jojGameService, jojGameStorageService, jojAIManager, $window, $localStorage, $ionicLoading, $rootScope, $log, $timeout, $state, ptzAds) {
            return {
                templateUrl: "app/Game/widgets/jojBoard/jojBoardTemplate.html",
                scope: {
                    boardOptions: "=",
                    players: "="
                },
                link: function (scope, element) {
                    ptzAds.create();

                    var w = angular.element($window);
                    var setBoardSize = function () {
                        var container = element[0].parentElement.parentElement;
                        var size = {
                            'h': container.offsetHeight,
                            'w': container.offsetWidth
                        };
                        scope.boardSize = (size.h > size.w ? size.w : size.h);
                        scope.menuWidth = (size.h > size.w ? size.w : size.w - size.h);
                        scope.menuHeight = (size.h > size.w ? size.h - size.w : size.h);
                    };

                    $timeout(function () {
                        setBoardSize();
                    });

                    w.bind("resize", function () {
                        setBoardSize();
                        scope.$apply();
                    });

                    scope.getPieceClass = function (piece) {
                        var cssClass = "";

                        if (piece > -1) {
                            cssClass += " joj-piece ";

                            if (piece === 0)
                                cssClass += " joj-piece-black ";
                            else if (piece === 1)
                                cssClass += " joj-piece-white ";
                        }

                        return cssClass;
                    }

                    scope.whereCanIGo = function (col) {
                        scope.currentPiece = col;
                        scope.game = jojGameService.whereCanIGo(scope.game, col);
                    }

                    var setPlayerTurn = function () {
                        if (scope.game.ended) {
                            scope.game.players.white.icon = !scope.game.blackWin ? "ion-trophy" : "ion-sad";
                            scope.game.players.black.icon = scope.game.blackWin ? "ion-trophy" : "ion-sad";
                            scope.helpOn = false;
                        } else {
                            scope.game.players.white.icon = scope.game.whiteTurn ? "ion-play" : "ion-pause";
                            scope.game.players.black.icon = !scope.game.whiteTurn ? "ion-play" : "ion-pause";
                            scope.helpOn = (scope.game.whiteTurn && scope.game.players.white.helpOn) || (!scope.game.whiteTurn && scope.game.players.black.helpOn);
                        }
                    }

                    var getComputerMove = function () {
                        jojAIManager.getComputerMove(scope.game).then(function (computerMove) {
                            if (computerMove) {
                                scope.game = jojGameService.move(scope.game, computerMove.startPosition, computerMove.nextPosition);
                                jojGameStorageService.setGame(scope.game);
                                setPlayerTurn();
                            }
                        });
                    }

                    var computerNeedsToPlay = function () {
                        if (scope.game.computerTurn) {
                            $ionicLoading.show({
                                template: '<ion-spinner icon="bubbles"></ion-spinner>'
                            });
                            $timeout(function () {
                                getComputerMove();
                                $ionicLoading.hide();
                            }, 1000);
                        }
                    }

                    scope.move = function (positionToGo) {
                        if (!scope.currentPiece || !positionToGo.iCanGoHere)
                            return;

                        scope.game = jojGameService.move(scope.game, scope.currentPiece, positionToGo);
                        jojGameStorageService.setGame(scope.game);

                        scope.currentPiece = {};

                        setPlayerTurn();
                        computerNeedsToPlay();
                    }

                    scope.newGameOnline = function () {
                        var players = {
                            white: { name: "White", foto: "img/white_user.png" },
                            black: { name: "Black", foto: "img/black_user.png" }
                        };
                        scope.newGameOffline(players);
                    }

                    var init = function () {
                        scope.game = jojGameStorageService.getGame();

                        if (!scope.game || !scope.game.players)
                            $state.go("game.new");
                        else {
                            setPlayerTurn();
                            computerNeedsToPlay();
                        }
                    }
                    init();

                    $rootScope.$on('jojGame.new', function () {
                        init();
                    });

                    scope.currentPiece = {};

                    scope.changeHelpOn = function () {
                        scope.helpOn = !scope.helpOn;
                        $localStorage.helpOn = scope.helpOn;
                    }

                    scope.backMove = function () {
                        scope.game = jojGameService.backMove(scope.game);
                        jojGameStorageService.setGame(scope.game);

                        scope.currentPiece = {};

                        setPlayerTurn();
                        computerNeedsToPlay();
                    }
                }
            }
        }
    ]);