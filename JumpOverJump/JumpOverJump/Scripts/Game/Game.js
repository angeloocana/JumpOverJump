JumpOverJump.Game = function(game) {
    this.background = null;
    this.jumpers = [];
    this.squares = [[]];
    this.squareWidth = 78;
    this.squareHeight = 78;
    this.boardX = 20;
    this.boardY = 20;
    this.ready = false;
    this.selectedJumperIndex = null;
};

JumpOverJump.Game.prototype = {	
	preload: function () {
		this.background = this.add.image(0, 0, 'background');
	},

	create: function () {
        this.buildSquares();
        this.buildJumpers();
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
        
        this.addSquare(x,y,square);            
    },
    
    addSquare: function(x, y, square){
        if(this.squares == undefined)
            this.squares = new Array();
        if(this.squares[x] == undefined)
            this.squares[x] = new Array();
        
        square.board = {x:x,y:y};
        this.squares[x][y] = square;
    },
    
    buildJumpers: function(){        
        var isblack = false;
        this.jumpers = [];
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
        jumper.board = {x:x,y:y};
        jumper.i = this.jumpers.length;
        this.jumpers[jumper.i] = jumper;
    },
        
    jumperOver: function (object, pointer) {
        //this.whereJumperCanGo(object);
    },

    jumperClicked: function(object, pointer) {
        this.whereJumperCanGo(object);        
    },
    
    whereJumperCanGo: function(jumper){
        console.log('whereJumperCanGo');
        var x = jumper.board.x;
        var y = jumper.board.y;
        this.selectedJumperIndex = jumper.i;
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
                var square = this.squares[x][y];
                square.animations.play(square.defaultAnimation, 1, false);
                square.events.onInputDown.remove(this.squareClicked, this);
            }	
        }	
        
        var jumper = this.getSelectedJumper();
        if(jumper)
        {            
            var square = this.squares[jumper.board.x][jumper.board.y];
            square.animations.play("on", 1, false);
        }
    },
    
    jumperCanGo: function(x,y,isAJump){             
        if(isAJump == true){
            console.log(' x: '+  x  + ' y:' + y);
        }
        
        isAJump = typeof isAJump !== 'undefined' ? isAJump : false;        
        var column = this.squares[x];
        if(column[y])
        {                
            //console.log('for  x:' + this.selectedJumper.square.x + ' y:'+ this.selectedJumper.square.y);
            for(var i = 0; i < this.jumpers.length; i++){                
                //console.log('jx: ' + this.jumpers[i].x +' x: '+  x +' jy: '+ this.jumpers[i].y + ' y:' + y);
                if(this.jumpers[i].board.x == x && this.jumpers[i].board.y == y)
                {
                    if(isAJump == false)
                    {                        
                        var jumper = this.getSelectedJumper();
                        var jx = jumper.board.x;
                        var jy = jumper.board.y;
                        
                        if(jx == x){
                            if(jy > y)
                                jy = y-1;   
                            else
                                jy = y+1; 
                        }                        
                        else if(jy == y){
                            if(jx > x)
                                jx = x-1; 
                            else
                                jx = x+1; 
                        }
                        
                        console.log('jx: ' + jx +' x: '+  x +' jy: '+ jy + ' y:' + y);
                        this.jumperCanGo(jx,jy,true);
                    }
                    return false;
                }
            }
            
            var square = column[y];
            square.animations.play("canGo", 1, false);
            square.events.onInputDown.add(this.squareClicked, this);
        } 
    },
    
    squareClicked: function(square, pointer) {
        var selectedJumper = this.getSelectedJumper();
        this.add.tween(selectedJumper).to({x:square.x, y:square.y}, 1000, Phaser.Easing.Quadratic.InOut, true);
        
        selectedJumper.board.x = square.board.x;
        selectedJumper.board.y = square.board.y;
        this.jumpers[selectedJumper.i] = selectedJumper;
        this.whereJumperCanGo(selectedJumper);
    },
    
    getSelectedJumper: function(){
        return this.jumpers[this.selectedJumperIndex];
    },

	update: function () {
	   	this.ready = true;
	}
};