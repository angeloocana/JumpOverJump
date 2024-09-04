"use strict";

angular.module("jojApp").directive("sgSignInLink",
    ["$location", function ($location) {
        return {
            templateUrl: "app/Users/widgets/sgSignIn/sgSignInLinkTemplate.html",
            scope: {},
            link:function(scope) {
                scope.getReturnUrl = function() {
                    return $location.url();
                }
            }
        }
    }
    ]);