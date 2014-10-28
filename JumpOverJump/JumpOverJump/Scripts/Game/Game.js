JumpOverJump.Game = function(game) {
    this.background = null;
    this.jumpers = null;
    this.squares = [];
    this.squareWidth = 78;
    this.squareHeight = 78;
    this.boardX = 20;
    this.boardY = 20;
    this.ready = false;
};

JumpOverJump.Game.prototype = {	
	preload: function () {
		this.background = this.add.image(0, 0, 'background');
	},

	create: function () {
        this.buildSquares();
	},
    
    getSquareKey : function(x,y){
        return 'square' + x + '_' + y;
    },
    
    createSquare: function (squareIndex, x, y, isblack) {
        var px = (x * this.squareWidth) + this.boardX;
        var py = (y * this.squareHeight) + this.boardY;
        var squareAnimation = isblack ? "black" : "white";
        var square = this.add.sprite(px, py, 'square');
        square.inputEnabled = true;
        square.animations.add('white'[0]);
        square.animations.add('black',[1]);
        square.animations.play(squareAnimation, 1, false);
        square.events.onInputDown.add(this.squareClicked, this);
        square.events.onInputOver.add(this.squareOver, this);
        square.events.onInputOut.add(this.squareOut, this);
        this.squares[squareIndex] = square;
    },
    
    squareOver: function (object, pointer) {

    object.alpha = 0.5;
},

squareOut: function(object, pointer) {
    object.alpha = 1;
},

squareClicked: function(object, pointer) {

        console.log('boom');

},
    
    buildSquares: function(){        
        var isblack = false;
        var squareIndex = 0;
        for(var y = 0; y < 8; y++){
            for(var x = 0; x < 8; x++){            
                this.createSquare(squareIndex, x, y, isblack);
                isblack = !isblack;
                squareIndex++;
            }	
            isblack = !isblack;
        }	
    },

	update: function () {
	   	this.ready = true;
	}
};