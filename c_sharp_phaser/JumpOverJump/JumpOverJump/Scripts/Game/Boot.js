var JumpOverJump = {};

JumpOverJump.Boot = function(game) {};

JumpOverJump.Boot.prototype = {
    preload: function() {
        this.load.image('background','../../Images/game/background.jpg');		
        this.load.atlasJSONHash('jumper', '../../assets/sprites/jumper.png', '../../assets/sprites/jumper.json');
        this.load.atlasJSONHash('square', '../../assets/sprites/square.png', '../../assets/sprites/square.json');
    },
    
    create: function() {
        this.input.maxPointers = 1;
		this.stage.disableVisibilityChange = false;
		this.scale.scaleMode = Phaser.ScaleManager.SHOW_ALL;
		this.scale.minWidth = 800;
		this.scale.minHeight = 600;
		this.scale.pageAlignHorizontally = true;
		this.scale.pageAlignVertically = true;
		this.stage.forcePortrait = true;
		this.scale.setScreenSize(true);

		this.input.addPointer();
		this.stage.backgroundColor = '#171642';
        
        this.state.start('Game');
    }
}