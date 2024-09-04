"use strict";

angular.module("jojApp").directive("sgCreateAccountLink",
    ["$location", function ($location) {
        return {
            templateUrl: "app/Users/widgets/sgCreateAccount/sgCreateAccountLinkTemplate.html",
            restrict: "E",
            scope: {
                showNotRegistred: "=showNotRegistred"
            },
            link: function (scope) {
                scope.getReturnUrl = function () {
                    return $location.url();
                }
            }
        }
    }
    ]);