"use strict";

angular.module("jojApp").controller("resetPasswordController", [
    "$scope", "$location", "$log", function ($scope, $location, $log) {
        var params = $location.search();
        $scope.id = params.id;
        $scope.token = params.token;
    }]);