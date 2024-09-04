"use strict";

angular.module("jojApp").controller("jojComputerPlayerController", [
    "$rootScope", "$scope", "jojGameStorageService", "jojGameService", "$state", "$localStorage", "$log", "$window", "ptzResponsive", "jojPlayerService",
    function ($rootScope, $scope, jojGameStorageService, jojGameService, $state, $localStorage, $log, $window, ptzResponsive, jojPlayerService) {
        $scope.players = $localStorage.computerPlayer;

        if (!$scope.players || !$scope.players.white || !$scope.players.black)
            $scope.players = jojPlayerService.getPlayersVsComputer(true);

        var setFotos = function () {
            if ($scope.players.computerIsWhite) {
                $scope.players.white.foto = "img/pc.png";
                $scope.players.black.foto = "img/black_user.png";
            }
            else {
                $scope.players.white.foto = "img/white_user.png";
                $scope.players.black.foto = "img/pc.png";
            }

            $scope.players.black.isComputer = !$scope.players.computerIsWhite;
            $scope.players.white.isComputer = $scope.players.computerIsWhite;
        }
        setFotos();

        $scope.switchPlayers = function () {
            $scope.players.computerIsWhite = !$scope.players.computerIsWhite;

            var whiteName = $scope.players.white.name;
            $scope.players.white.name = $scope.players.black.name;
            $scope.players.black.name = whiteName;

            setFotos();
        }

        $scope.play = function () {
            $localStorage.computerPlayer = $scope.players;
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
