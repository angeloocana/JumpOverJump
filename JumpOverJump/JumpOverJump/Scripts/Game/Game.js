JumpOverJump.Game = function(game) {
    this.background = null;
    this.jumpers = null;
    this.squares = [[]];
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
        this.buildJumpers();
	},
    
    getSquareKey : function(x,y){
        return 'square' + x + '_' + y;
    },
    
    createSquare: function (x, y, isblack) {
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
        
        this.addSquare(x,y,square,null);            
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
        for(var x = 0; x < 8; x++){            
            for(var y = 0; y < 8; y++){
                this.createSquare(x, y, isblack);
                isblack = !isblack;
            }	
            isblack = !isblack;
        }	
    },
    
    createJumper: function (x, y, isblack) {
        var px = (x * this.squareWidth) + this.boardX;
        var py = (y * this.squareHeight) + this.boardY;
        var squareAnimation = isblack ? "black" : "white";
        var jumper = this.add.sprite(px, py, 'jumper');
        jumper.inputEnabled = true;
        jumper.animations.add('white'[0]);
        jumper.animations.add('black',[1]);
        jumper.animations.play(squareAnimation, 1, false);
        jumper.events.onInputDown.add(this.squareClicked, this);
        jumper.events.onInputOver.add(this.squareOver, this);
        jumper.events.onInputOut.add(this.squareOut, this);
        
        this.addSquare(x,y,null,jumper);            
    },
    
    addSquare: function(x, y, square, jumper){
        if(this.squares == undefined)
            this.squares = new Array();
        if(this.squares[x] == undefined)
            this.squares[x] = new Array();
        
        this.squares[x][y] = {
            square : square == null ? this.squares[x][y].square : square, 
            jumper : jumper
        };
    },
    
    buildJumpers: function(){        
        var isblack = false;
        for(var y = 0; y < 8; y = y + 7){            
            for(var x = 0; x < 8; x++){                        
                this.createJumper(x, y, isblack);
            }	
            isblack = !isblack;
        }	
    },

	update: function () {
	   	this.ready = true;
	}
};