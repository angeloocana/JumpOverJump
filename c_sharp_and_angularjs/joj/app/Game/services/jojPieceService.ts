"use strict";

angular.module("jojApp").factory("jojPieceService",
    [function () {
            
            //Tested
            var getStartPieces = function (boardOptions, row) {
                var pieces = [];
                for (var x = 0; x < boardOptions.size.x; x++) {
                    var piece = {
                        x: x,
                        y: row
                    };
                    pieces.push(piece);
                }
                return pieces;
            }

            //Tested
            var isBlackPiece = function (position) {
                if (!position || position.piece < 0)
                    return null;
                return position.piece === 0;
            }

            return {
                getStartPieces: getStartPieces,
                isBlackPiece: isBlackPiece
            };
        }]);