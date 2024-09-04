"use strict";

angular.module("jojApp").directive("sgLoader",
    [function () {
        return {
            templateUrl: "app/SharedKernel/directives/sgLoader/sgLoaderTemplate.html",
            scope: {
                isLoading: "="
            }
        }
    }
    ]);