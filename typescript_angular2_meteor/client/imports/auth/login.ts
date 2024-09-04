import { Component } from '@angular/core';
import { MeteorComponent } from 'angular2-meteor';
import { FormBuilder, ControlGroup, Validators } from '@angular/common';
import { Router , ROUTER_DIRECTIVES} from '@angular/router';
import { Meteor } from 'meteor/meteor';
 
import template from './login.html';

@Component({
  selector: 'login',
  directives: [ROUTER_DIRECTIVES],
  template: template
})
export class Login extends MeteorComponent {
  loginForm: ControlGroup;
  error: string;
 
  constructor(private router: Router) {
    super();
 
    let fb = new FormBuilder();
 
    this.loginForm = fb.group({
      email: ['', Validators.required],
      password: ['', Validators.required]
    });
 
    this.error = '';
  }
 
  login(event) {
    let credentials = this.loginForm.value;
    if (credentials && credentials.email && credentials.password) {
      Meteor.loginWithPassword(credentials.email, credentials.password, (err) => {
        if (err) {
          this.error = err;
          alert('err');
          alert(err);
        }
        else {
          alert('this.router.navigate([]);');
          this.router.navigate(['/']);
        }
      });
    }
    else{
      alert('Invalid Form');
      console.log('this.loginForm.value');
      console.log(this.loginForm.value);
    }

    event.preventDefault();
  }  
}