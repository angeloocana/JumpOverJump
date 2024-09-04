import { Component } from '@angular/core';
import { MeteorComponent } from 'angular2-meteor';
import { FormBuilder, ControlGroup, Validators } from '@angular/common';
import { Router , ROUTER_DIRECTIVES} from '@angular/router';
import { Accounts } from 'meteor/accounts-base';
 
import template from './recover.html';
 
@Component({
  selector: 'recover',
  directives: [ROUTER_DIRECTIVES],
  template
})
export class Recover extends MeteorComponent {
  recoverForm: ControlGroup;
  error: string;
 
  constructor(private router: Router) {
    super();
 
    let fb = new FormBuilder();
 
    this.recoverForm = fb.group({
      email: ['', Validators.required]
    });
 
    this.error = '';
  }
 
  recover(credentials) {
    if (this.recoverForm.valid) {
      Accounts.forgotPassword({ email: credentials.email}, (err) => {
        if (err) {
          this.error = err;
        }
        else {
          this.router.navigate(['/']);
        }
      });
    }
  }
}