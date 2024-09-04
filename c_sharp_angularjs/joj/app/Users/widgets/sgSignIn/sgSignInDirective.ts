"use strict";

angular.module("jojApp").directive("sgSignIn",
    [function () {
        return {
            templateUrl: "app/Users/widgets/sgSignIn/sgSignInTemplate.html",
            controller: ["$scope", "userService", "$timeout", "$location", "$log", "$rootScope", function ($scope, userService, $timeout, $location, $log, $rootScope) {
                $scope.isLoading = false;
                $scope.sendSingInForm = function () {
                    $scope.isLoading = true;
                    userService.signIn($scope.user).then(function (data) {
                        if (data.success)
                            userService.redirectToPreviousPage();
                        else
                            $scope.errors = data.errors;

                        $timeout(function () {
                            $scope.isLoading = false;
                        });
                    });;
                }

                $rootScope.$on("authorize.changed", function () {
                    if (userService.isAuthorized())
                        userService.redirectToPreviousPage();
                });
            }]
        }
    }
    ]);