"use strict";

angular.module("jojApp").factory("jojAIService",
    ["jojGameService", "jojBoardService", "$log",
        function (jojGameService, jojBoardService, $log) {
            var getOtherPieces = function (pieces, remove) {
                var newPieces = [];
                for (var i = 0; i < pieces.length; i++) {
                    var piece = pieces[i];
                    if (piece && (piece.x !== remove.x || piece.y !== remove.y))
                        newPieces.push(piece);
                }

                return newPieces;
            }

            var getRandom = function (min, max) {
                return Math.floor(Math.random() * max) + min;
            }

            var getGameColor = function (game) {
                var black = !game.players.computerIsWhite;
                return black ? game.black : game.white;
            }

            var getCopy = function (object) {
                return JSON.parse(JSON.stringify(object));
            }

            var addBestMoviment = function (bestMoviments, positionsAHead, startPosition, nextPosition, nivel, isBlack) {
                if (!bestMoviments)
                    bestMoviments = [];

                if (!bestMoviments[nivel])
                    bestMoviments[nivel] = [];

                var positionsAHeadOnMoviment = jojBoardService.getY0Start7End(nextPosition.y, isBlack) - jojBoardService.getY0Start7End(startPosition.y, isBlack);

                var indexPositionsAHead = positionsAHead + positionsAHeadOnMoviment;

                if (!bestMoviments[nivel][indexPositionsAHead])
                    bestMoviments[nivel][indexPositionsAHead] = [];

                for (var i = 0; i < bestMoviments[nivel][indexPositionsAHead].length; i++) {
                    var bestMoviment = bestMoviments[nivel][indexPositionsAHead][i];
                    if (bestMoviment.startPosition.x === startPosition.x && bestMoviment.startPosition.y === startPosition.y
                        && bestMoviment.nextPosition.x === nextPosition.x && bestMoviment.nextPosition.y === nextPosition.y)
                        return bestMoviments;
                }

                if (indexPositionsAHead < 0)
                    return bestMoviments;

                if (!bestMoviments[nivel][indexPositionsAHead])
                    bestMoviments[nivel][indexPositionsAHead] = [];

                bestMoviments[nivel][indexPositionsAHead].push({
                    startPosition: startPosition,
                    nextPosition: nextPosition
                });

                return bestMoviments;
            }
            
            var getBestNivel = function (bestMoviments) {
                var bestNivel = 0;
                for (var k = 0; k < bestMoviments.length; k++) {
                    var bestMoves = bestMoviments[k];
                    var bestMovesBestNivel = bestMoviments[bestNivel];

                    if (bestMoves.length > bestMovesBestNivel.length
                        && bestMoves[bestMoves.length - 1].length > 0)
                        bestNivel = k;
                }

                return bestNivel;
            }

            var getPiecesOrdered = function (pieces, isBlack) {
                var ordered = [];
                for (var i = 0; i < pieces.length; i++) {
                    var piece = pieces[i];
                    var y = jojBoardService.getY0Start7End(piece.y, isBlack);
                    if (!ordered[y])
                        ordered[y] = [piece];
                    else
                        ordered[y].push(piece);
                }

                return ordered;
            }
            
            var fillPiecesWhereCanTheyGo = function (game, pieces, isBlack) {
                if (!pieces) {
                    isBlack = !game.whiteTurn;
                    pieces = isBlack ? game.black.pieces : game.white.pieces;
                }

                for (var i = 0; i < pieces.length; i++) {
                    pieces[i].whereCanIGo = jojBoardService.getPositionsWhereCanIGo(pieces[i], game, isBlack);
                }

                return pieces;
            }

            var getWinPosition = function (winPositions, x, isBlack) {
                var winPosition = winPositions[x];
                if (winPosition && winPosition.length > 0 && winPosition[0].nextPosition) {
                    for (var i = 0; i < winPosition.length; i++) {
                        if (winPosition[i].nextPosition) {
                            var move = winPosition[i].nextPosition.lastMoviment ? winPosition[i].nextPosition.lastMoviment : winPosition[i];
                            if (jojBoardService.getY0Start7End(move.startPosition.y < 7, isBlack))
                                return move;
                        }
                    }
                }
            }

            var getDistance = function (piece, isBlack) {
                var winPositions = [];
                for (var x = 0; x < 8; x++) {
                    var y = jojBoardService.getY7Start0End(piece.y, isBlack);
                    var xDistance = x - piece.x;
                    if (xDistance < 0)
                        xDistance = xDistance * -1;
                    var moviments = xDistance > y || y === 0 ? xDistance : y;

                    if (moviments > 1) {
                        for (var i = 0; i < piece.whereCanIGo.positions.length; i++) {
                            var nextPosition = piece.whereCanIGo.positions[i];
                            var nextPositionY = jojBoardService.getY7Start0End(nextPosition.y, isBlack);
                            var nextPositionXDistance = x - nextPosition.x;
                            if (nextPositionXDistance < 0)
                                nextPositionXDistance = nextPositionXDistance * -1;

                            var nextPositionMoviments = (nextPositionXDistance > nextPositionY || nextPositionY === 0 ? nextPositionXDistance : nextPositionY) + 1;
                            if (nextPositionMoviments < moviments)
                                moviments = nextPositionMoviments;
                        }
                    }

                    winPositions[x] = moviments < 0 ? moviments * -1 : moviments;
                }

                return winPositions;
            }

            var getNMovimentsToWin = function (pieces, isBlack) {
                var winPositions = [];
                for (var i = 0; i < pieces.length; i++) {
                    var piece = pieces[i];
                    piece.movimentsToWin = getDistance(piece, isBlack);
                }

                for (var x = 0; x < 8; x++) {
                    if (!winPositions[x])
                        winPositions[x] = [];

                    for (var j = 0; j < pieces.length; j++) {
                        var piece = pieces[j];
                        if (!winPositions[x][piece.movimentsToWin[x]])
                            winPositions[x][piece.movimentsToWin[x]] = [];
                        winPositions[x][piece.movimentsToWin[x]].push(piece);
                    }
                }

                return winPositions;
            }

            var isGameEnding = function (allPieces, isBlack) {
                for (var i = 0; i < allPieces.length; i++) {
                    var piece = allPieces[i];
                    if (jojBoardService.getY0Start7End(piece.y, isBlack) < 4)
                        return false;
                }

                return true;
            }

            var getLastMovimentToWin = function (nMovimentsToWin) {
                for (var i = 0; i < nMovimentsToWin.length; i++) {
                    if (nMovimentsToWin[i])
                        return i;
                }

                return 10;
            }

            var getPiecesFurtherAway = function (pieces, isBlack) {
                var lastMovimentToWin = -1;
                var nMovimentsToWin = getNMovimentsToWin(pieces, isBlack);
                for (var i = 0; i < nMovimentsToWin.length; i++) {
                    var nMoves = getLastMovimentToWin(nMovimentsToWin[i]);
                    if (nMoves > lastMovimentToWin)
                        lastMovimentToWin = nMoves;
                }
                return lastMovimentToWin;
            }

            var getBestMoviment = function (game, moves, isBlack) {
                var bestMoviment = [];
                for (var i = 0; i < moves.length; i++) {
                    var gameCopy = getCopy(game);
                    var move = moves[i];
                    gameCopy = jojGameService.getGameAfterMove(gameCopy, move.startPosition, move.nextPosition);
                    var pieces = getGameColor(gameCopy).pieces;
                    pieces = fillPiecesWhereCanTheyGo(gameCopy, pieces, isBlack);
                    var furtherPiece = getPiecesFurtherAway(pieces, isBlack);
                    if (!bestMoviment[furtherPiece])
                        bestMoviment[furtherPiece] = [];

                    bestMoviment[furtherPiece].push(move);
                }

                for (var j = 0; j < bestMoviment.length; j++) {
                    if (bestMoviment[j])
                        return bestMoviment[j][getRandom(1, bestMoviment[j].length) - 1];
                }
            }

            var getMoveNearWinPosition = function (allPieces, moviments, orderedPieces, winPositions, isBlack, game) {
                var winMove = undefined;

                if (isGameEnding(allPieces, isBlack)) {
                    if (winPositions && winPositions.length > 0) {
                        winMove = getWinPosition(winPositions, 0, isBlack);
                        if (winMove) return winMove;
                        winMove = getWinPosition(winPositions, 7, isBlack);
                        if (winMove) return winMove;
                    }

                    //winMove = getWinPositionOrganized(winPositions, isBlack);
                    //if (winMove) return winMove;

                    winMove = getWinPosition(winPositions, 1, isBlack);
                    if (winMove) return winMove;
                    winMove = getWinPosition(winPositions, 3, isBlack);
                    if (winMove) return winMove;
                    winMove = getWinPosition(winPositions, 5, isBlack);
                    if (winMove) return winMove;
                }

                var winPositionEmpty = [];
                var winAndPreWinPositionEmpty = [];

                if (orderedPieces[7]) {
                    for (var i = 0; i < moviments.length; i++) {
                        var move = moviments[i];
                        var empty = true;
                        var emptyPreWin = true;

                        for (var j = 0; j < orderedPieces[7].length; j++) {
                            var piece = orderedPieces[7][j];
                            if (piece && piece.x === move.x) {
                                empty = false;
                            }
                        }

                        if (orderedPieces[6])
                            for (var j = 0; j < orderedPieces[6].length; j++) {
                                var piece = orderedPieces[6][j];
                                if (piece && piece.x === move.x) {
                                    emptyPreWin = false;
                                }
                            }
                        if (empty) {
                            winPositionEmpty.push(move);
                            if (emptyPreWin)
                                winAndPreWinPositionEmpty.push(move);
                        }
                    }
                }

                var positions = winAndPreWinPositionEmpty.length > 0 ? winAndPreWinPositionEmpty : winPositionEmpty;


                var choosenMove = getBestMoviment(game, (positions.length > 0 ? positions : moviments), isBlack);
                return choosenMove;
            }

            var addWinPositions = function (startPosition, nextPosition, winPositions, nivel, isBlack) {
                if (!nextPosition) {
                    if (jojBoardService.getY0Start7End(startPosition.y, isBlack) === 7) {
                        if (!winPositions[startPosition.x])
                            winPositions[startPosition.x] = [];

                        for (var i = 0; i < winPositions[startPosition.x].length; i++) {
                            var winPosition = winPositions[startPosition.x][i];
                            if (winPosition.startPosition.x === startPosition.x &&
                                winPosition.startPosition.y === startPosition.y)
                                return winPositions;
                        }

                        winPositions[startPosition.x].push({
                            startPosition: startPosition,
                            nivel: nivel
                        });
                    }
                } else {
                    if (jojBoardService.getY0Start7End(nextPosition.y, isBlack) === 7) {
                        if (!winPositions[nextPosition.x])
                            winPositions[nextPosition.x] = [];

                        for (var i = 0; i < winPositions[nextPosition.x].length; i++) {
                            var winPosition = winPositions[nextPosition.x][i];
                            if (winPosition.startPosition.x === startPosition.x &&
                                winPosition.startPosition.y === startPosition.y)
                                return winPositions;
                        }

                        winPositions[nextPosition.x].push({
                            startPosition: startPosition,
                            nextPosition: nextPosition,
                            nivel: nivel
                        });
                    }
                }
                return winPositions;
            }

            var addBestMovimentsAfterMove = function (piece, startPositionY, startPositionBeforeMove, nextPositionBeforeMove, game, isBlack, bestMoviments, winPositions) {
                var gameCopy = getCopy(game);
                gameCopy = jojGameService.getGameAfterMove(gameCopy, startPositionBeforeMove, nextPositionBeforeMove);

                var whereCanIGoAfterMove = jojBoardService.getPositionsWhereCanIGo(piece, gameCopy, isBlack);
                if (piece.whereCanIGo.orderedPositions.length < whereCanIGoAfterMove.orderedPositions.length) {
                    var higherPositions = whereCanIGoAfterMove.orderedPositions[whereCanIGoAfterMove.orderedPositions.length - 1];
                    for (var p = 0; p < higherPositions.length; p++) {
                        var nextPosition = higherPositions[p];
                        if (nextPosition) {
                            nextPosition.lastMoviment = { startPosition: startPositionBeforeMove, nextPosition: nextPositionBeforeMove };
                            var nextPositionY = jojBoardService.getY0Start7End(nextPosition.y, isBlack);
                            bestMoviments = addBestMoviment(bestMoviments, (nextPositionY - startPositionY), startPositionBeforeMove, nextPositionBeforeMove, 1, isBlack);
                            winPositions = addWinPositions(piece, nextPosition, winPositions, 1, isBlack);
                        }
                    }
                }

                return bestMoviments;
            }

            var getBestMove = function (game) {
                var pcColor = getGameColor(game);
                var isBlack = !game.players.computerIsWhite;
                var bestMoviments = [];
                var winPositions = [];

                var allPieces = fillPiecesWhereCanTheyGo(game, pcColor.pieces, isBlack);
                getNMovimentsToWin(allPieces, isBlack);

                var orderedPieces = getPiecesOrdered(allPieces, isBlack);
                
                for (var l = 0; l < orderedPieces.length; l++) {
                    if (orderedPieces[l]) {
                        var pieces = orderedPieces[l];
                        for (var m = 0; m < pieces.length; m++) {
                            var piece = pieces[m];
                            
                            addWinPositions(piece, null, winPositions, -1, isBlack);
                            var startPositionY = jojBoardService.getY0Start7End(piece.y, isBlack);

                            var nextPositions = piece.whereCanIGo.orderedPositions[piece.whereCanIGo.orderedPositions.length - 1];
                            if (nextPositions)
                            for (var i = 0; i < nextPositions.length; i++) {
                                var nextPosition = nextPositions[i];
                                if (nextPosition) {
                                    var nextPositionY = jojBoardService.getY0Start7End(nextPosition.y, isBlack);
                                    
                                    bestMoviments = addBestMoviment(bestMoviments, (nextPositionY - startPositionY), piece, nextPosition, 0, isBlack);
                                    bestMoviments = addBestMovimentsAfterMove(piece, startPositionY, piece, nextPosition, game, isBlack, bestMoviments, winPositions, allPieces);

                                    winPositions = addWinPositions(piece, nextPosition, winPositions, 0, isBlack);
                                }
                            }

                            if (piece.whereCanIGo.orderedPositions.length < 8) {
                                var otherPieces = getOtherPieces(allPieces, piece);
                                for (var n = 0; n < otherPieces.length; n++) {
                                    var otherPiece = otherPieces[n];
                                    for (var o = 0; o < otherPiece.whereCanIGo.positions.length; o++) {
                                        var otherPieceMove = otherPiece.whereCanIGo.positions[o];
                                        bestMoviments = addBestMovimentsAfterMove(piece, startPositionY, otherPiece, otherPieceMove, game, isBlack, bestMoviments, winPositions, allPieces);

                                        winPositions = addWinPositions(otherPiece, otherPieceMove, winPositions, 0, isBlack);
                                    }
                                }
                            }
                        }
                    }
                }

                var bestNivel = getBestNivel(bestMoviments);
                var higherBestMoviments = bestMoviments[bestNivel][bestMoviments[bestNivel].length - 1];

                var move = getMoveNearWinPosition(allPieces, higherBestMoviments, orderedPieces, winPositions, isBlack, game);
                return move;
            }

            var getComputerMove = function (game) {              

                if (game.ended)
                    return null;

                if (!game.players.vsComputer)
                    return null;

                if (game.players.computerIsWhite !== game.whiteTurn)
                    return null;

                return getBestMove(game);
            }

            return {
                getComputerMove: getComputerMove,
                getRandom: getRandom,
                getBestMove: getBestMove,
                fillPiecesWhereCanTheyGo: fillPiecesWhereCanTheyGo
            };
        }]);