"use strict";

angular.module("jojApp").factory("jojBoardService",
    ["jojPieceService", function (jojPieceService) {

        //Tested
        var isBackGroundBlack = function (x, y) {
            if (x % 2 === 0) {
                if (y % 2 === 0)
                    return true;
                else
                    return false;
            } else {
                if (y % 2 === 0)
                    return false;
                else
                    return true;
            }
        }
        
        //Tested
        var fillPiecesOnBoard = function (board, pieces, value) {
            for (var i = 0; i < pieces.length; i++) {
                var piece = pieces[i];
                board[piece.x][piece.y].piece = value;
            }

            return board;
        }

        //Tested
        var generateBoard = function (game) {
            var board = [];
            for (var x = 0; x < game.boardOptions.size.x; x++) {
                for (var y = 0; y < game.boardOptions.size.y; y++) {
                    if (!board[x])
                        board[x] = [];

                    var position = {
                        x: x,
                        y: y,
                        backgroundBlack: isBackGroundBlack(x, y),
                        whiteHome: y === game.boardOptions.size.y - 1,
                        blackHome: y === 0,
                        piece: -1
                    };

                    board[x][y] = position;
                }
            }

            board = fillPiecesOnBoard(board, game.white.pieces, 1);
            board = fillPiecesOnBoard(board, game.black.pieces, 0);

            return board;
        }

        //Tested
        var boardHasThisPosition = function (position, boardOptions) {
            return (position.x < 0 || position.y < 0 || position.x >= boardOptions.size.x || position.y >= boardOptions.size.y)
                ? false : true;
        }

        //Tested
        var getPosition = function (position, board) {
            return board[position.x][position.y];
        }

        //Tested
        var isPositionEmpty = function (position, board) {
            return !(getPosition(position, board).piece > -1);
        }

        //Tested
        var getNearPositions = function (position, onlyEmpty, game) {
            var positions = [];
            var add = function (plusX, plusY) {
                var newPosition = { x: position.x + plusX, y: position.y + plusY }

                if (!boardHasThisPosition(newPosition, game.boardOptions))
                    return;

                if (typeof onlyEmpty != "undefined") {
                    var positionEmpty = isPositionEmpty(newPosition, game.board);
                    if (onlyEmpty === positionEmpty)
                        positions.push(newPosition);
                } else
                    positions.push(newPosition);
            }

            add(-1, -1);
            add(0, -1);
            add(+1, -1);

            add(-1, 0);
            add(+1, 0);

            add(-1, +1);
            add(0, +1);
            add(+1, +1);

            return positions;
        }

        //Tested
        var getJumpPosition = function (startPosition, toJumpPosition, game) {
            var jumpPosition = {};

            if (startPosition.x < toJumpPosition.x)
                jumpPosition.x = toJumpPosition.x + 1;
            else if (startPosition.x > toJumpPosition.x)
                jumpPosition.x = toJumpPosition.x - 1;
            else jumpPosition.x = toJumpPosition.x;

            if (startPosition.y < toJumpPosition.y)
                jumpPosition.y = toJumpPosition.y + 1;
            else if (startPosition.y > toJumpPosition.y)
                jumpPosition.y = toJumpPosition.y - 1;
            else jumpPosition.y = toJumpPosition.y;

            if (boardHasThisPosition(jumpPosition, game.boardOptions) && isPositionEmpty(jumpPosition, game.board))
                return jumpPosition;
        }

        //Tested
        var isPositionNotAdded = function (position, positions) {
            for (var j = 0; j < positions.length; j++) {
                if (position.x === positions[j].x && position.y === positions[j].y)
                    return false;
            }

            return true;
        }

        var getIndexToSearchOrder = function (x) {
            switch (x) {
                case 0:
                    return 0;
                case 1:
                    return 2;
                case 2:
                    return 4;
                case 3:
                    return 6;
                case 4:
                    return 7;
                case 5:
                    return 5;
                case 6:
                    return 3;
                case 7:
                    return 1;
                default:
                    return null;
            }
        }

        //Tested
        var getY0Start7End = function (y, isBlack) {
            if (isBlack)
                return y;

            switch (y) {
                case 0:
                    return 7;
                case 1:
                    return 6;
                case 2:
                    return 5;
                case 3:
                    return 4;
                case 4:
                    return 3;
                case 5:
                    return 2;
                case 6:
                    return 1;
                case 7:
                    return 0;
                default:
                    return null;
            }
        }

        //Tested
        var getY7Start0End = function (y, isBlack) {
            if (!isBlack)
                return y;

            switch (y) {
                case 0:
                    return 7;
                case 1:
                    return 6;
                case 2:
                    return 5;
                case 3:
                    return 4;
                case 4:
                    return 3;
                case 5:
                    return 2;
                case 6:
                    return 1;
                case 7:
                    return 0;
                default:
                    return null;
            }
        }
        
        var whereCanIJump = function (jumpStartPosition, positions, game, orderedPositions, isBlack) {
            var nearFilledPositions = getNearPositions(jumpStartPosition, false, game);
            for (var k = 0; k < nearFilledPositions.length; k++) {
                var nearFilledPosition = nearFilledPositions[k];

                var jumpPosition = getJumpPosition(jumpStartPosition, nearFilledPosition, game);
                if (jumpPosition) {
                    if (isPositionNotAdded(jumpPosition, positions)) {
                        jumpPosition.lastPosition = jumpStartPosition;
                        jumpPosition.jumpingBlackPiece = jojPieceService.isBlackPiece(nearFilledPosition);
                        jumpPosition.jumps = jumpStartPosition.jumps ? jumpStartPosition.jumps++ : 2;

                        positions.push(jumpPosition);
                        var y = getY0Start7End(jumpPosition.y, isBlack);
                        if (!orderedPositions[y])
                            orderedPositions[y] = [];
                        orderedPositions[y][getIndexToSearchOrder(jumpPosition.x)] = jumpPosition;

                        whereCanIJump(jumpPosition, positions, game, orderedPositions, isBlack);
                    }
                }
            }
        }

        var getPositionsWhereCanIGo = function (startPosition, game, isBlack) {
            if (!startPosition)
                return startPosition;

            var allNearPositions = getNearPositions(startPosition, undefined, game);
            var positions = [];
            var orderedPositions = [];

            for (var i = 0; i < allNearPositions.length; i++) {
                var nearPosition = allNearPositions[i];
                if (isPositionEmpty(nearPosition, game.board)) {
                    positions.push(nearPosition);

                    var y = getY0Start7End(nearPosition.y, isBlack);
                    if (!orderedPositions[y])
                        orderedPositions[y] = [];

                    orderedPositions[y][getIndexToSearchOrder(nearPosition.x)] = nearPosition;
                } else {
                    var jumpPosition = getJumpPosition(startPosition, nearPosition, game);
                    if (jumpPosition) {
                        jumpPosition.jumps = 1;
                        positions.push(jumpPosition);

                        var y = getY0Start7End(jumpPosition.y, isBlack);
                        if (!orderedPositions[y])
                            orderedPositions[y] = [];
                        orderedPositions[y][getIndexToSearchOrder(jumpPosition.x)] = jumpPosition;

                        whereCanIJump(jumpPosition, positions, game, orderedPositions, isBlack);
                    }
                }
            }

            return {
                positions: positions,
                orderedPositions: orderedPositions
            }
        }

        var cleanBoardWhereCanIGo = function (board) {
            for (var x = 0; x < board.length; x++) {
                for (var y = 0; y < board[x].length; y++) {
                    board[x][y].iCanGoHere = false;
                    board[x][y].lastMove = false;
                    board[x][y].lastMoveJump = false;
                }
            }
            return board;
        }

        return {
            isBackGroundBlack: isBackGroundBlack,
            fillPiecesOnBoard: fillPiecesOnBoard,
            generateBoard: generateBoard,
            boardHasThisPosition: boardHasThisPosition,
            getY0Start7End:getY0Start7End,
            getY7Start0End: getY7Start0End,
            cleanBoardWhereCanIGo: cleanBoardWhereCanIGo,
            getPositionsWhereCanIGo: getPositionsWhereCanIGo,
            getPosition: getPosition,
            isPositionEmpty: isPositionEmpty,
            getNearPositions: getNearPositions,
            getJumpPosition: getJumpPosition,
            isPositionNotAdded: isPositionNotAdded
        };
    }]);