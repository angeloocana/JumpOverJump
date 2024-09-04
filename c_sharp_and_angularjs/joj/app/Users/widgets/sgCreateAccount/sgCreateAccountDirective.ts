"use strict";

angular.module("jojApp").directive("sgCreateAccount",
    [function () {
        return {
            templateUrl: "app/Users/widgets/sgCreateAccount/sgCreateAccountTemplate.html",
            scope: {},
            controller: ["$scope", "userService", "$timeout", function ($scope, userService, $timeout) {
                $scope.isLoading = false;
                $scope.sendCreateAccountForm = function () {
                    $scope.isLoading = true;
                    userService.createAccount($scope.newUser).then(function (data) {
                        if (data.success)
                            userService.redirectToPreviousPage();
                        else
                            $scope.errors = data.errors;

                        $timeout(function () {
                            $scope.isLoading = false;
                        });
                    });;
                }
            }]
        }
    }
    ]);