"use strict";

angular.module("jojApp").factory("jojPlayerService",
    [function () {

        var getDefaultPlayer = function () {
            return {
                name: "",
                color: "WHITE",
                helpOn: true
            }
        }

        var getComputerPlayer = function (color) {
            return { name: "PC", foto: "http://jojsc.azurewebsites.net/img/pc.png", color: color, isComputer: true }
        }

        var getPlayersVsComputer = function (computerIsWhite) {
            return {
                black: computerIsWhite ? getDefaultPlayer() : getComputerPlayer("BLACK"),
                white: computerIsWhite ? getComputerPlayer("WHITE") : getDefaultPlayer(),
                vsComputer: true,
                computerVsComputer: true,
                computerIsWhite: computerIsWhite
            }
        }

        return {
            getComputerPlayer: getComputerPlayer,
            getPlayersVsComputer: getPlayersVsComputer
        };
    }]);