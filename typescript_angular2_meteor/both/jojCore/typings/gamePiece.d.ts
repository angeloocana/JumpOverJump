declare namespace jojCore{
    interface IGamePiece{
        x:number;
        y:number;

        whereCanIGo?: IPositionsWhereCanIGo;
        movimentsToWin?: number[];
    } 
}