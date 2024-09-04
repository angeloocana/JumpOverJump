import { Component } from '@angular/core';
import { ionicBootstrap, App, Platform, MenuController } from 'ionic-angular';
import { METEOR_PROVIDERS } from 'angular2-meteor';
import { GameNew } from '../game/new/game-new';

import template from './app.mobile.html';

@Component({
    template
})
class Socially {
rootPage: any;
  constructor(app: App, platform: Platform, menu: MenuController) {
    this.rootPage = GameNew;
  }
}

export function run(): void {
    ionicBootstrap(Socially, [
        METEOR_PROVIDERS
    ]);
}