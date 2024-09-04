"use strict";

angular.module("jojApp").factory("ptzAds",
    ["admobSvc", function (admobSvc) {
       var create = function () {
               admobSvc.createBannerView();
        };

        return {
            create: create
        }
    }])