"use strict";

angular.module("jojApp").controller("jojNewGameController", [
    "$scope", "ptzResponsive", "$window",   "$log",
    function ($scope, ptzResponsive, $window, $log) {
        var w = angular.element($window);

        var setBtnsContaneirClass = function () {
            $scope.btnsContaneirClass = ptzResponsive.returnOnlyFor("", " col-50 col-offset-25 ", " col-33 col-offset-33 ");
        };

        w.bind("resize", function () {
            setBtnsContaneirClass();
            $scope.$apply();
        });
        setBtnsContaneirClass();
    }]);
