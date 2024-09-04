"use strict";

angular.module("jojApp").directive("ptzHref",
    [function () {
        return {
            restrict: 'A',
            link: function (scope, elem, attrs) {
                if (typeof cordova !== 'undefined') {
                    attrs.$set("onclick", "window.open('" + attrs.ptzHref + "', '_system', 'location=yes'); return false;");
                } else {
                    attrs.$set("href", attrs.ptzHref);
                    attrs.$set("target", "_blank");
                }
            }
        }
    }]);