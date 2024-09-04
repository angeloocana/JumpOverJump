"use strict";

angular.module("jojApp").directive("sgResetPassword",
    ["userService", function (userService) {
        return {
            templateUrl: "app/Users/widgets/sgResetPassword/sgResetPasswordTemplate.html",
            scope: {
                id: "=",
                token:"="
            },
            link: function(scope) {
                scope.resetPassword = function() {
                    scope.resetPasswordModel.id = scope.id;
                    scope.resetPasswordModel.token = scope.token;

                    return userService.resetPassword(scope.resetPasswordModel, function (data) {
                        if (data.success) {
                            scope.passwordChanged = true;
                        } else {
                            scope.passwordChanged = false;
                            scope.errors = data.errors;
                        }
                    });
                }
            }
        }
    }
    ]);