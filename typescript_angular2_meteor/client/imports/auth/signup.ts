import { Component } from '@angular/core';
import { MeteorComponent } from 'angular2-meteor';
import { FormBuilder, ControlGroup, Validators } from '@angular/common';
import { Router , ROUTER_DIRECTIVES} from '@angular/router';
import { Accounts } from 'meteor/accounts-base';
 
import template from './signup.html';
 
@Component({
  selector: 'signup',
  directives: [ROUTER_DIRECTIVES],
  template
})
export class Signup extends MeteorComponent {
  signupForm: ControlGroup;
  error: string;
 
  constructor(private router: Router) {
    super();
 
    let fb = new FormBuilder();
 
    this.signupForm = fb.group({
      email: ['', Validators.required],
      password: ['', Validators.required]
    });
 
    this.error = '';
  }
 
  signup(credentials) {
    if (this.signupForm.valid) {
      Accounts.createUser({ email: credentials.email, password: credentials.password}, (err) => {
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