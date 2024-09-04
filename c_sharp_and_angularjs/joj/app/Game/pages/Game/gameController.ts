"use strict";

angular.module("jojApp").controller("gameController", [
    "$scope", "$log", function ($scope, $log) {
        if (typeof analytics !== 'undefined') {
            analytics.trackView("GameController"); 
        }
    }]);