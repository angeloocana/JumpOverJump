var gameParams = {
    game: {
         width: 1000, height: 650
    },
     board: {
         square: { width: 78, height: 78 },
         borderX: 10,
         borderY:10
     }
};

var game = new Phaser.Game(gameParams.game.width, gameParams.game.height, Phaser.AUTO, '', { preload: preload, create: create, update: update });

var board = function() {
    var doForEachSquare = function(toDo){
        for(var x = 0; x < 8; x++){
            for(var y = 0; y < 8; y++){
                toDo(x,y);
            }	
        }	
    };

    var getSquareKey = function(x,y){
        return 'square' + x + '_' + y;
    };

    var load = function() {
        doForEachSquare(function(x, y) {
            game.load.spritesheet(getSquareKey(x, y), 'Images/game/square.png', 78, 78);
        });
    };
    
    var create = function () {

        doForEachSquare(function (x, y) {
            var px = (x * gameParams.board.square.width) + gameParams.board.borderX;
            var py = (y * gameParams.board.square.height) + gameParams.board.borderY;
            game.add.sprite(px, py, getSquareKey(x, y));
        });
    };

    return {
        load : load,
        doForEachSquare : doForEachSquare,
        create:create
    }
}();

function preload() {
    game.load.image('background','Images/game/background.jpg');		
    game.load.spritesheet('jumper','Images/game/jumper.png');
    board.load();				
}	

function create() {
    game.add.sprite(0, 0, 'background');	
    game.add.sprite(300, 300, 'jumper');
    board.create();
}

function update() {
}


