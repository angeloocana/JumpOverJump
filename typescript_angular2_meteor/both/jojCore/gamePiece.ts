namespace jojCore {
    export class GamePiece implements IGamePiece {
        x: number;
        y: number;

        whereCanIGo: IPositionsWhereCanIGo;
    }
}