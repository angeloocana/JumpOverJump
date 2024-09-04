"use strict";

angular.module("jojApp").directive("ptzExternalLoginLinks",
    ["$location", function ($location) {
        return {
            templateUrl: "app/Users/widgets/ptzExternalLoginLinks/ptzExternalLoginLinksTemplate.html",
            link:function(scope) {
                scope.returnUrl = $location.path();
            }
        }
    }
    ]);