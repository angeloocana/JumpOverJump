JumpOverJump.Game = function(game) {
    this.background = null;
    this.jumpers = null;
    this.squares = [[]];
    this.squareWidth = 78;
    this.squareHeight = 78;
    this.boardX = 20;
    this.boardY = 20;
    this.ready = false;
    this.selectedJumper = null;
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
        var square = this.add.sprite(px, py, 'square');
        square.inputEnabled = true;
        square.animations.add('white', [0]);
        square.animations.add('black', [1]);
        square.animations.add('canGo', [2]);
        square.animations.add('on', [3]);
        square.defaultAnimation = isblack ? "black" : "white";
        square.animations.play(square.defaultAnimation, 1, false);                
        
        this.addSquare(x,y,square,null);            
    },

    squareClicked: function(object, pointer) {
        console.log(object.x);
        console.log(object.y);
        console.log(object);
        this.add.tween(this.selectedJumper).to({x:object.x, y:object.y}, 1000, Phaser.Easing.Quadratic.InOut, true);
        this.resetSquaresAnimation();
        this.squares[this.selectedJumper.square.x][this.selectedJumper.square.y].jumper = null;
        this.squares[x][y].jumper = this.selectedJumper; 
        this.squares[x][y].jumper.x = x;
        this.squares[x][y].jumper.y = y;
        this.selectedJumper = null;
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
    
    buildJumpers: function(){        
        var isblack = false;
        for(var y = 0; y < 8; y = y + 7){            
            for(var x = 0; x < 8; x++){                        
                this.createJumper(x, y, isblack);
            }	
            isblack = !isblack;
        }	
    },
    
    createJumper: function (x, y, isblack) {
        var px = (x * this.squareWidth) + this.boardX;
        var py = (y * this.squareHeight) + this.boardY;
        var jumperAnimation = isblack ? "jblack" : "jwhite";
        var jumper = this.add.sprite(px, py, 'jumper');
        jumper.inputEnabled = true;
        jumper.animations.add('jwhite'[0]);
        jumper.animations.add('jblack',[1]);
        jumper.animations.play(jumperAnimation, 1, false);
        jumper.events.onInputDown.add(this.jumperClicked, this);
        jumper.events.onInputOver.add(this.jumperOver, this);
        jumper.square = {x:x,y:y};
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
    
    jumperOver: function (object, pointer) {
        this.whereJumperCanGo(object);
    },

    jumperClicked: function(object, pointer) {
        this.whereJumperCanGo(object);        
    },
    
    whereJumperCanGo: function(jumper){
        var x = jumper.square.x;
        var y = jumper.square.y;
        this.selectedJumper = jumper;
        this.resetSquaresAnimation();
        
        if(this.squares[x-1])        
        {            
            this.jumperCanGo(x-1,y-1);   
            this.jumperCanGo(x-1,y);   
            this.jumperCanGo(x-1,y+1);   
        }
            
        this.jumperCanGo(x,y-1);   
        this.jumperCanGo(x,y+1);   
         
        if(this.squares[x+1])        
        {            
            this.jumperCanGo(x+1,y-1);   
            this.jumperCanGo(x+1,y);   
            this.jumperCanGo(x+1,y+1);   
        }
    },
    
    resetSquaresAnimation : function(){
        for(var x = 0; x < 8; x++){     
            for(var y = 0; y < 8; y++){                        
                var square = this.squares[x][y].square;
                square.animations.play(square.defaultAnimation, 1, false);
                square.events.onInputDown.remove(this.squareClicked, this);
            }	
        }	
    },
    
    jumperCanGo: function(x,y){
        var column = this.squares[x];
        if(column[y])
            {
                var jumper = column[y].jumper;
                var square = column[y].square;
                if(jumper == null || jumper == undefined){
                    square.animations.play("canGo", 1, false);
                    square.events.onInputDown.add(this.squareClicked, this);
                }
            } 
    },

	update: function () {
	   	this.ready = true;
	}
};