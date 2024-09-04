"use strict";

angular.module("jojApp").factory("ptzResponsive",
    ["$log", "$window",
    function ($log, $window) {
        var width = $window.innerWidth;

        var returnOnlyForMobile = function (cssClasses) {
            if (width < 567)
                return cssClasses;
            return "";
        }

        var returnOnlyForTablet = function (cssClasses) {
            if (width < 900 && width > 567)
                return cssClasses;
            return "";
        }

        var returnOnlyForTabletAndPC = function (cssClasses) {
            if (width > 900)
                return cssClasses;
            else if (width > 567)
                return cssClasses;
            return "";
        }

        var returnOnlyFor = function (cssMobile, cssTablet, cssPc) {
            if (width > 900)
                return cssPc;
            else if (width > 567)
                return cssTablet;
            return cssMobile;
        }

        var returnOnlyForPC = function (cssClasses) {
            if (width > 900)
                return cssClasses;
            return "";
        }

        return {
            returnOnlyForTabletAndPC: returnOnlyForTabletAndPC,
            returnOnlyForPC: returnOnlyForPC,
            returnOnlyForMobile: returnOnlyForMobile,
            returnOnlyForTablet: returnOnlyForTablet,
            returnOnlyFor: returnOnlyFor
        };
    }]);