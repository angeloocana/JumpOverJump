import {MeteorComponent} from 'angular2-meteor';
import { Component, provide } from '@angular/core';
import { ionicBootstrap } from 'ionic-angular';
import { METEOR_PROVIDERS } from 'angular2-meteor';
import { provideRouter, RouterConfig, ROUTER_DIRECTIVES } from '@angular/router';
import {HTTP_PROVIDERS} from '@angular/http';
import {TRANSLATE_PROVIDERS, TranslateService, TranslatePipe, TranslateLoader, TranslateStaticLoader} from 'ng2-translate/ng2-translate';
import { APP_BASE_HREF } from '@angular/common';
import {LoginButtons} from 'angular2-meteor-accounts-ui';
import { InjectUser } from 'angular2-meteor-accounts-ui';
import { Login } from '../auth/login';
import { Signup } from '../auth/signup';
import { Recover } from '../auth/recover';
import { Meteor } from 'meteor/meteor';
import { GameNew } from '../game/new/game-new';

import template from './app.web.html';

@Component({
  template,
  directives: [ROUTER_DIRECTIVES, LoginButtons],
  pipes: [TranslatePipe]  
})
@InjectUser()
export class Socially extends MeteorComponent {
  user: Meteor.User;

  constructor(translate: TranslateService) {
    super();

    let userLang = navigator.language.split('-')[0];
        userLang = /(pt|en)/gi.test(userLang) ? userLang : 'pt';
        translate.setDefaultLang('en');
        translate.use(userLang);
  }

  logout() {
    this.autorun(() => {
      Meteor.logout();
    });
  }
}

const routes: RouterConfig = [
  { path: '',              	component: GameNew },
  { path: 'login', 		component: Login },
  { path: 'signup', 		component: Signup },
  { path: 'recover', 		component: Recover }
];

const APP_ROUTER_PROVIDERS = [
  provideRouter(routes)
];

export function run(): void {
  ionicBootstrap(Socially, [HTTP_PROVIDERS,METEOR_PROVIDERS, TRANSLATE_PROVIDERS, APP_ROUTER_PROVIDERS, provide(APP_BASE_HREF, { useValue: '/' })]);
}