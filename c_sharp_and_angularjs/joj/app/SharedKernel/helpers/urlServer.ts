"use strict";

angular.module("jojApp").factory("urlServer",
    [function () {
        return function (url) {
            if (!window.location.origin) {
                window.location.origin = window.location.protocol + "//" + window.location.hostname + (window.location.port ? ":" + window.location.port : "");
            }

            return window.location.origin + "/" + url;
        }
    }]);