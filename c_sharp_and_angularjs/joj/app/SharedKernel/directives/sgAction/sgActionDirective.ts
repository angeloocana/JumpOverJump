"use strict";

angular.module("jojApp").directive("sgAction",
    ["$timeout", "$log", function ($timeout, $log) {
        return {
            templateUrl: "app/SharedKernel/directives/sgAction/sgActionTemplate.html",
            scope: {
                errors: "=",
                action: "=",
                actionForm: "=",
                actionLabel: "="
            },
            link: function (scope, element, attributes, ngModel) {
                scope.startAction = function () {
                    scope.isLoading = true;
                    scope.action().then(function () {
                        $timeout(function () {
                            scope.isLoading = false;
                        });
                    });
                }
            }
        }
    }]);