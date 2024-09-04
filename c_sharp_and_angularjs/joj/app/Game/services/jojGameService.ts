"use strict";

angular.module("jojApp").factory("jojGameService",
    ["$log", "jojBoardService", "jojPieceService", "jojColorService",
        function ($log, jojBoardService, jojPieceService, jojColorService) {

            //Tested
            var isComputerTurn = function (game) {
                if (game.ended)
                    return false;

                if (!game.players)
                    return undefined;

                if (!game.players.vsComputer)
                    return false;
                
                return game.players.computerIsWhite === game.whiteTurn;
            }

            var getDefaultGameIfItsNull = function (game) {
                if (game)
                    return game;

                return {
                    boardOptions: { size: { x: 8, y: 8 } }
                };          
            }

            //Tested
            var getCleanGame = function (game) {
                game = getDefaultGameIfItsNull(game);

                if (!game.boardOptions)
                    game.boardOptions = { size: { x: 8, y: 8 } };                   

                game.movements = [];
                game.white = jojColorService.getCleanColor(game.white, game.boardOptions, false);
                game.black = jojColorService.getCleanColor(game.black, game.boardOptions, true);

                game.ended = false;
                game.whiteTurn = true;

                game.board = jojBoardService.generateBoard(game);
                
                game.computerTurn = isComputerTurn(game);
                return game;
            }

            //Tested
            var resetGame = function (game, players) {
                game = getDefaultGameIfItsNull(game);

                game.players = players;
                game.players.white.color = "WHITE";
                game.players.black.color = "BLACK";
                return getCleanGame(game);
            }

            var whereCanIGo = function (game, startPosition) {
                game.board = jojBoardService.cleanBoardWhereCanIGo(game.board);
                var blackPiece = jojPieceService.isBlackPiece(startPosition);

                if (game.ended || blackPiece === null || (!blackPiece && !game.whiteTurn) || (blackPiece && game.whiteTurn))
                    return game;

                var positions = jojBoardService.getPositionsWhereCanIGo(startPosition, game, blackPiece).positions;

                for (var i = 0; i < positions.length; i++) {
                    var position = positions[i];
                    game.board[position.x][position.y].iCanGoHere = true;
                }

                return game;
            }

            var getColorWinners = function (color) {
                color.winners = 0;
                color.preWinnersPoints = 0;

                for (var i = 0; i < color.pieces.length; i++) {
                    var piece = color.pieces[i];
                    if (piece.y === color.endRow)
                        color.winners++;
                    else
                        color.preWinnersPoints += color.endRow === 0 ? color.startRow - piece.y : piece.y;
                }

                return color;
            }

            var verifyEnd = function (game, color) {
                if (color.winners === game.boardOptions.size.y) {
                    game.ended = true;
                    //Save to backend
                    return true;
                }

                return false;
            }

            var verifyWinner = function (game, nextPosition) {
                game.white = getColorWinners(game.white, nextPosition);
                game.black = getColorWinners(game.black, nextPosition);

                if (verifyEnd(game, game.white))
                    game.blackWin = false;
                else if (verifyEnd(game, game.black))
                    game.blackWin = true;

                return game;
            }

            var getCopy = function (object) {
                return JSON.parse(JSON.stringify(object));
            }

            var getPiecesAfterMove = function (pieces, startPosition, nextPosition) {
                var piecesCopy = getCopy(pieces);

                for (var i = 0; i < piecesCopy.length; i++) {
                    if (piecesCopy[i].x === startPosition.x && piecesCopy[i].y === startPosition.y) {
                        piecesCopy[i].x = nextPosition.x;
                        piecesCopy[i].y = nextPosition.y;
                        return piecesCopy;
                    }
                }
                return piecesCopy;
            }

            var getGameAfterMove = function (game, startPosition, nextPosition, backMove) {
                if (backMove) {
                    game.board[nextPosition.x][nextPosition.y].piece = game.whiteTurn ? 0 : 1;
                    game.board[startPosition.x][startPosition.y].piece = -1;
                } else {
                    game.board[nextPosition.x][nextPosition.y].piece = game.board[startPosition.x][startPosition.y].piece;
                    game.board[startPosition.x][startPosition.y].piece = -1;
                }

                var jumpPosition = nextPosition.lastPosition;
                while (jumpPosition) {
                    game.board[jumpPosition.x][jumpPosition.y].lastMoveJump = true;
                    jumpPosition = jumpPosition.lastPosition;
                }

                game.board[nextPosition.x][nextPosition.y].lastMove = true;
                game.board[startPosition.x][startPosition.y].lastMove = true;

                if (jojPieceService.isBlackPiece(game.board[nextPosition.x][nextPosition.y]))
                    game.black.pieces = getPiecesAfterMove(game.black.pieces, startPosition, nextPosition);
                else
                    game.white.pieces = getPiecesAfterMove(game.white.pieces, startPosition, nextPosition);

                if (!backMove) {
                    game.movements.push({ startPosition: startPosition, nextPosition: nextPosition });
                    game = verifyWinner(game, nextPosition);
                }

                game.whiteTurn = !game.whiteTurn;
                
                game.computerTurn = isComputerTurn(game);

                return game;
            }

            var move = function (game, startPosition, nextPosition) {                
                var positionsWhereCanIGo = jojBoardService.getPositionsWhereCanIGo(startPosition, game, !game.whiteTurn).positions;
                var nextPositionFound = false;
                for (var i = 0; i < positionsWhereCanIGo.length; i++) {
                    var position = positionsWhereCanIGo[i];
                    if (position.x === nextPosition.x && position.y === nextPosition.y) {
                        nextPositionFound = true;
                        nextPosition = position;
                    }
                }

                game.board = jojBoardService.cleanBoardWhereCanIGo(game.board);

                if (!nextPositionFound)
                    return game;

                game = getGameAfterMove(game, startPosition, nextPosition);
                
                return game;
            }

            var backMove = function (game) {
                game.board = jojBoardService.cleanBoardWhereCanIGo(game.board);
                
                var lastMove = game.movements.pop();

                if (lastMove)
                    game = getGameAfterMove(game, lastMove.nextPosition, lastMove.startPosition, true);

                if (game.players.vsComputer) {
                    lastMove = game.movements.pop();
                    if (lastMove) {
                        game.board = jojBoardService.cleanBoardWhereCanIGo(game.board);
                        game = getGameAfterMove(game, lastMove.nextPosition, lastMove.startPosition, true);
                    }
                }
                                
                return game;
            }

            var newGame = function (game, players) {
                return resetGame(game, players);
            }

            return {
                getCleanGame: getCleanGame,
                resetGame: resetGame,
                move: move,
                whereCanIGo: whereCanIGo,
                newGame: newGame,
                getGameAfterMove: getGameAfterMove,
                backMove: backMove,
                isComputerTurn: isComputerTurn
            };
        }]);