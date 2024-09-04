"use strict";

angular.module("jojApp").controller("jojOfflinePlayersController", [
    "$rootScope", "$scope", "jojGameStorageService", "jojGameService", "$state", "$localStorage", "$log", "$window", "ptzResponsive",
    function ($rootScope, $scope, jojGameStorageService, jojGameService, $state, $localStorage, $log, $window, ptzResponsive) {
        $scope.players = $localStorage.offLinePlayers;

        if (!$scope.players || !$scope.players.white || !$scope.players.black)
            $scope.players = {
                white: {
                    name: "",
                    color: "WHITE",
                    helpOn: true
                },
                black: {
                    name: "",
                    color: "BLACK",
                    helpOn: true
                }
            };

        //if (!$scope.players.white.foto)
            $scope.players.white.foto = "img/white_user.png";

        //if (!$scope.players.black.foto)
            $scope.players.black.foto = "img/black_user.png";

        $scope.switchPlayers = function () {
            var whiteName = $scope.players.white.name;
            $scope.players.white.name = $scope.players.black.name;
            $scope.players.black.name = whiteName;
        }

        $scope.play = function () {
            $localStorage.offLinePlayers = $scope.players;
            var game = jojGameService.newGame(jojGameStorageService.getGame(), $scope.players);
            jojGameStorageService.setGame(game);
            $rootScope.$emit('jojGame.new');
            $state.go("game.play");
        }
        
        var w = angular.element($window);

        var setBtnsContaneirClass = function () {
            var firstCol = ptzResponsive.returnOnlyForPC(" col-25 col-offset-25 ");
            var secondCol = ptzResponsive.returnOnlyForPC(" col-25 ");

            $scope.firstCol = firstCol;
            $scope.secondCol = secondCol;
        };
        
        w.bind("resize", function () {
            setBtnsContaneirClass();
            $scope.$apply();
        });
        setBtnsContaneirClass();
    }]);
