"use strict";

angular.module("jojApp").factory("userService",
    ["$http", "$q", "$log", "urlServer", "$localStorage", "$rootScope", "$location", "$translate",
        function ($http, $q, $log, urlServer, $localStorage, $rootScope, $location, $translate) {

        var getAuthorizeType = function (info) {
            switch (info) {
                case undefined:
                    return "unauthorized";
                case null:
                    return "unauthorized";
                default:
                    return "authorized";
            }
        };

        var userInfo = function (user) {
            if (user !== undefined) {
                if (user !== null && user.success) {
                    $localStorage.userInfo = user.return;
                } else $localStorage.userInfo = null;

                $rootScope.$emit("authorize.changed");
            }
            return $localStorage.userInfo;
        };

        var request = function (url, data, onSuccess, onError) {
            var deferred = $q.defer();
            $http({ method: "POST", url: urlServer("api/" + url), data: data }).
                success(function (data, status, headers, config) {
                    if (onSuccess)
                        onSuccess(data);

                    deferred.resolve(data);
                }).
                error(function (data, status, headers, config) {
                    if (onError)
                        onError(data);

                    deferred.resolve(data);
                });
            return deferred.promise;
        };

        var getUserInfo = function (reCheck) {
            var deferred = $q.defer();
            if (reCheck === undefined && userInfo() !== undefined)
                deferred.resolve(userInfo());
            else {
                $http({ method: "POST", url: urlServer("api/users/user/getInfo") }).
                    success(function (data, status, headers, config) {
                        userInfo(data);
                        deferred.resolve(userInfo());
                    }).
                    error(function (data, status, headers, config) {
                        userInfo({ success: false });
                        deferred.reject(status);
                    });
            }
            return deferred.promise;
        };

        var iForgotMyPassword = function (data, onSuccess, onError) {
            data.langKey = $translate.use();
            return request("Users/User/ForgotMyPassword", data, onSuccess, onError);
        };

        var resetPassword = function (data, onSuccess, onError) {
            return request("Users/User/ResetPassword", data, onSuccess, onError);
        };

        var signOut = function () {
            var deferred = $q.defer();
            $http({ method: "POST", url: urlServer("api/users/user/logoff") }).
                success(function (data, status, headers, config) {
                    $rootScope.$emit("authorize.signOut");
                    deferred.resolve(userInfo(null));
                }).
                error(function (data, status, headers, config) {
                    deferred.resolve(userInfo(null));
                });
            return deferred.promise;
        }

        var redirectToPreviousPage = function () {
            var returnUrl = $localStorage.returnUrl;
            $localStorage.redirectedFromLogin = true;
            $location.path(returnUrl ? returnUrl : "/");
        }

        var redirectToLogin = function () {
            $localStorage.returnUrl = $location.url();
            $location.path("/signIn");
        }

        return {
            redirectToPreviousPage: redirectToPreviousPage,
            redirectToLogin:redirectToLogin,
            getUserInfo: getUserInfo,
            getAuthorize: function () {
                return getAuthorizeType(userInfo());
            },
            isAuthorized: function () {
                return this.getAuthorize() === "authorized";
            },
            signOut: signOut,
            createAccount: function (newUser) {
                var deferred = $q.defer();
                $http({ method: "POST", url: urlServer("api/users/user/register"), data: newUser }).
                    success(function (data, status, headers, config) {
                        userInfo(data);
                        if (data.success)
                            $rootScope.$emit("authorize.signin");
                        deferred.resolve(data);
                    }).
                    error(function (data, status, headers, config) {
                        userInfo(data);
                        deferred.resolve(data);
                    });
                return deferred.promise;
            },
            signIn: function (user) {
                var deferred = $q.defer();
                $http({ method: "POST", url: urlServer("api/users/user/signIn"), data: user }).
                    success(function (data, status, headers, config) {
                        userInfo(data);
                        if (data.success)
                            $rootScope.$emit("authorize.signin");
                        deferred.resolve(data);
                    }).
                    error(function (data, status, headers, config) {
                        userInfo(data);
                        deferred.resolve(data);
                    });
                return deferred.promise;
            },
            iForgotMyPassword: iForgotMyPassword,
            resetPassword: resetPassword
        };
    }]);