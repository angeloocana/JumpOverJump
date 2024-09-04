"use strict";

angular.module("jojApp").factory("languageService",
    ["$timeout", function ($timeout) {

        var version = 1;
        var getLastVersion = function () {
            
        };

        return {
            getLastVersion: getLastVersion
        }
    }])