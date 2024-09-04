angular.module("jojApp")
     .config(["$locationProvider",
        function ($locationProvider) {
                $locationProvider.html5Mode(true);
        }]);