"use strict";

angular.module("jojApp").directive("sgErrors",
    [function () {
        return {
            templateUrl: "app/SharedKernel/widgets/sgErrors/sgErrorsTemplate.html",
            scope: "="
        }
    }
    ]);