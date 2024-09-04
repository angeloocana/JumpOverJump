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



var board = function() {
    var doForEachSquare = function (toDo) {
        var isblack = false;
        for(var x = 0; x < 8; x++){
            for(var y = 0; y < 8; y++){
                toDo(x, y, isblack);
                isblack = !isblack;
            }	
        }	
    };

    var getSquareKey = function(x,y){
        return 'square' + x + '_' + y;
    };

    var load = function() {
        doForEachSquare(function (x, y, isblack) {
            //game.load.spritesheet(getSquareKey(x, y), 'Images/game/square.png', 78, 78);
            game.load.atlasJSONHash(getSquareKey(x, y), '../../assets/sprites/square.png', '../../assets/sprites/square.json');
        });
    };
    
    var create = function () {

        doForEachSquare(function (x, y, isblack) {
            var px = (x * gameParams.board.square.width) + gameParams.board.borderX;
            var py = (y * gameParams.board.square.height) + gameParams.board.borderY;
            game.add.sprite(px, py, getSquareKey(x, y));
            var animation = isblack ? "black" : "white";
            game.animations.add(animation);
            game.animations.play(animation, 1, false);
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


