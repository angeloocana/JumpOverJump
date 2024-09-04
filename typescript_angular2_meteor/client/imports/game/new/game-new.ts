import { Component }   from '@angular/core';
import { TranslatePipe } from 'ng2-translate/ng2-translate';

import template from './game-new.html';
 
@Component({
  template,
  pipes: [TranslatePipe]
})
export class GameNew {
  constructor() {
  }
}