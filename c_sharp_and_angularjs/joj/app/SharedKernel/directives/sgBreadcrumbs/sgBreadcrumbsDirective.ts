"use strict";

angular.module("jojApp").directive("sgBreadcrumbs",
    [function () {
        return {
            templateUrl: "app/SharedKernel/directives/sgBreadcrumbs/sgBreadcrumbsTemplate.html",
            scope: {
                pages: "="
            }
        }
    }
    ]);