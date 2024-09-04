import { Component }   from '@angular/core';
import { TranslatePipe } from 'ng2-translate/ng2-translate';

import template from './game-new-offline.html';

@Component({
    template,
    pipes: [TranslatePipe]
})
export class GameNewOffline {

    players: jojCore.IPlayers;

    constructor() {
        this.players = new jojCore.Players();
    }
}