"use strict";

angular.module("jojApp").factory("languageService",
    ["$timeout", function ($timeout) {
        var languages = [
            {
                name: "Português",
                key: "pt-br",
                icon: "flag-br"
            },
            {
                name: "English",
                key: "en-us",
                icon: "flag-usa"
            }
        ];

        var getLanguages = function () {
            return $timeout(function() {
                return languages;
            });
        };

        return {
            getLanguages: getLanguages
        }
    }])