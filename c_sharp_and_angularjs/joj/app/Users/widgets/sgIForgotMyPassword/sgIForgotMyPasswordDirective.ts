"use strict";

angular.module("jojApp").directive("sgIForgotMyPassword",
    ["userService",function (userService) {
        return {
            templateUrl: "app/Users/widgets/sgIForgotMyPassword/sgIForgotMyPasswordTemplate.html",
            scope: {

            },
            link: function (scope) {
                scope.openForm = function () {
                    scope.emailSent = false;
                    scope.open = !scope.open;
                }
                scope.send = function () {
                    return userService.iForgotMyPassword({ email: scope.email }, function (data) {
                        if (data.success) {
                            scope.emailSent = true;
                            scope.errors = [];
                        }
                        else
                            scope.errors = data.errors;
                    });
                };
            }
        }
    }
    ]);