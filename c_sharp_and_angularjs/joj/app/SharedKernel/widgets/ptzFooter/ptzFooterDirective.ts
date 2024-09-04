"use strict";

angular.module("jojApp").directive("ptzFooter",
    ["$log",
        function ($log) {
            return {
                templateUrl: "app/SharedKernel/widgets/ptzFooter/ptzFooterTemplate.html",
                restrict: "E",
                scope: {},
                link: function (scope, element) {
                  
                }
            }
        }
    ]);