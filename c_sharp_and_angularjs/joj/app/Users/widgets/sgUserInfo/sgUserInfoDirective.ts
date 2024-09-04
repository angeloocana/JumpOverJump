"use strict";

angular.module("jojApp").directive("sgUserInfo",
    [
        function () {
            return {
                templateUrl: "app/Users/widgets/sgUserInfo/sgUserInfoTemplate.html",
                controller: ["userService", "$scope", function (userService, $scope) {
                    userService.getUserInfo().then(function (userInfo) {
                        $scope.userInfo = userInfo;
                    });
                }
                ]
            };
        }]);