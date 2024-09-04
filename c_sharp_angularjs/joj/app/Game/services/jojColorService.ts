"use strict";

angular.module("jojApp").factory("jojColorService",
    ["$log", "jojPieceService",
        function ($log, jojPieceService) {

            //Tested
            var getCleanColor = function (color, boardOptions, isBlack) {
                if (!color)
                    color = {};

                color.winners = 0;
                color.jumps = 0;
                color.points = 0;
                color.nMoves = 0;
                
                var y = (boardOptions.size.y - 1);
                color.startRow = isBlack ? 0 : y;
                color.endRow = isBlack ? y : 0;

                color.pieces = jojPieceService.getStartPieces(boardOptions, color.startRow);

                return color;
            }

            return {
                getCleanColor: getCleanColor
            };
        }]);