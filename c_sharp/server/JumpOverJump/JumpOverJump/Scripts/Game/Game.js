JumpOverJump.Game = function(game) {
    this.background = null;
    this.jumpers = [];
    this.squares = [[]];
    this.squareWidth = 78;
    this.squareHeight = 78;
    this.board = {x:20, y:20, nSquareX:6, nSquareY:6};
    this.ready = false;
    this.selectedJumperIndex = null;
    this.isWhiteTurn = true;
    this.isJumping = false;
    this.jumpSquares = [];
    this.btnDone = null;
};

JumpOverJump.Game.prototype = {	
	preload: function () {
		this.background = this.add.image(0, 0, 'background');        
	},

	create: function () {
        this.buildSquares();
        this.buildJumpers();
        this.buildBtnDone();
	},
    
    changeTurn: function(){
        this.isWhiteTurn = !this.isWhiteTurn;
        this.btnDone.text = this.isWhiteTurn ? "White turn" : "Black turn";
    },
    
    buildBtnDone: function(){
        this.btnDone = this.add.text(700, 300, "White turn", { font: "35px Arial", fill: "#ff0044", align: "center" });
        this.btnDone.inputEnabled = true;
        this.btnDone.events.onInputDown.add(this.btnDoneClicked, this);    
    },
    
    btnDoneClicked: function(btn, pointer){
        if(this.isJumping)
        {
            this.changeTurn();
            this.isJumping = false;
            this.resetSquaresAnimation();
        }
        
        if(this.isWhiteTurn == null){
            this.state.start('Game');
        }
    },
    
    buildSquares: function(){        
        var isblack = false;
        for(var x = 0; x < this.board.nSquareX; x++){            
            for(var y = 0; y < this.board.nSquareY; y++){
                this.createSquare(x, y, isblack);
                isblack = !isblack;
            }	
            isblack = !isblack;
        }	
    },
    
    createSquare: function (x, y, isblack) {
        var px = (x * this.squareWidth) + this.board.x;
        var py = (y * this.squareHeight) + this.board.y;
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
        for(var y = 0; y < this.board.nSquareY; y = y + (this.board.nSquareY - 1)){            
            for(var x = 0; x < this.board.nSquareX; x++){                        
                this.createJumper(x, y, isblack);
            }	
            isblack = !isblack;
        }	
    },
    
    createJumper: function (x, y, isBlack) {
        var px = (x * this.squareWidth) + this.board.x;
        var py = (y * this.squareHeight) + this.board.y;
        var jumperAnimation = isBlack ? "black" : "white";
        var jumper = this.add.sprite(px, py, 'jumper');
        jumper.inputEnabled = true;
        jumper.animations.add('white'[0]);
        jumper.animations.add('black',[1]);
        jumper.animations.play(jumperAnimation, 1, false);
        jumper.events.onInputDown.add(this.jumperClicked, this);
        jumper.events.onInputOver.add(this.jumperOver, this);
        jumper.board = {x:x,y:y};
        jumper.i = this.jumpers.length;
        jumper.isBlack = isBlack;
        this.jumpers[jumper.i] = jumper;
    },
        
    jumperOver: function (object, pointer) {
        //this.whereJumperCanGo(object);
    },

    jumperClicked: function(jumper, pointer) {
        if(this.isWhiteTurn == jumper.isBlack)
            if(this.isJumping == false || this.selectedJumperIndex == null || this.selectedJumperIndex == jumper.i)
                this.whereJumperCanGo(jumper);        
    },
    
    whereJumperCanGo: function(jumper){
        if(this.isJumping == false)
            this.jumpSquares = [];
        
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
        for(var x = 0; x < this.board.nSquareX; x++){     
            for(var y = 0; y < this.board.nSquareY; y++){                        
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
        if(x < 0 || y < 0 || x >= this.squares.length || y >= this.squares.length)
            return;
        
        isAJump = typeof isAJump !== 'undefined' ? isAJump : false;        
        //console.log('x:' + x + ' y:' + y);
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
                        
                        if(x > jx){ //Coluna a direita
                            if(jy == y)
                                jx = x+1;
                            else if(y > jy)//Linha de baixo
                            {
                                jx = x + 1;    
                                jy = y + 1;
                            }
                            else if(y < jy) //Linha de cima
                            {
                                jx = x + 1;    
                                jy = y - 1;
                            }
                        }
                        else if(x < jx){ //Coluna a esquerda
                            if(jy == y)
                                jx = x-1;
                            else if(y > jy)//Linha de baixo
                            {
                                jx = x - 1;    
                                jy = y + 1;
                            }
                            else if(y < jy)//Linha de cima
                            {
                                jx = x - 1;    
                                jy = y - 1;
                            }
                        }
                        else if(x == jx){ //mesa coluna
                            if(y < jy)//Linha de cima
                                jy = y-1;   
                            else if(y > jy)//Linha de baixo
                                jy = y+1; 
                        }
                        
                        //console.log('jx: ' + jx +' x: '+  x +' jy: '+ jy + ' y:' + y);
                        this.jumperCanGo(jx,jy,true);
                    }
                    return false;
                }
            }
            
            if(this.isJumping == true && isAJump == false)
                return;
            
            if(isAJump)
                this.jumpSquares[this.jumpSquares.length] = {x:x,y:y};
            
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
        
        if(this.isAJumpingSquare(square.board.x,square.board.y))
        {
            this.isJumping = true;
            this.btnDone.text = "I'm done";
            this.whereJumperCanGo(selectedJumper);
        }
        else
        {
            this.changeTurn();
            this.isJumping = false;
            this.selectedJumperIndex = null;
            this.resetSquaresAnimation();
        }
        
        this.checkIfWeHaveAWinner();
    },
    
    isAJumpingSquare: function(x,y){
        for(var i = 0; i < this.jumpSquares.length; i++){
            //console.log('x: '+ x + ' y:' + y);
            if(this.jumpSquares[i].x == x && this.jumpSquares[i].y == y)
                return true;
        }
        return false;
    },
    
    getSelectedJumper: function(){
        return this.jumpers[this.selectedJumperIndex];
    },
    
    checkIfWeHaveAWinner: function(){
        var nWhiteJumpersOnTheFinalSquare = 0;
        var nBlackJumpersOnTheFinalSquare = 0;
        for(var i = 0; i < this.jumpers.length; i++){
            var jumper = this.jumpers[i];
            if(this.isJumperOnTheFinalSquare(jumper)){
                if(jumper.isBlack)
                    nBlackJumpersOnTheFinalSquare++;      
                else
                    nWhiteJumpersOnTheFinalSquare++;
            }

            if(nWhiteJumpersOnTheFinalSquare == this.board.nSquareX){
                this.weHaveAWinner(false);
            }
            else if(nBlackJumpersOnTheFinalSquare == this.board.nSquareY){
                this.weHaveAWinner(true);
            }
        }
    },    
        
    isJumperOnTheFinalSquare: function(jumper)    {
        if(jumper.board.y == (jumper.isBlack ? 0 : this.board.nSquareY - 1  )){
            this.squares[jumper.board.x][jumper.board.y].animations.play("canGo", 1, false);                
            return true;
        }            
        
        return false;
    },
        
    weHaveAWinner: function(isBlack){
        this.btnDone.text = (isBlack ? "White" : "Black") + " WON!!!!!!!  \n Play again?";
        this.isWhiteTurn = null;
    },

	update: function () {
	   	this.ready = true;
	}
};