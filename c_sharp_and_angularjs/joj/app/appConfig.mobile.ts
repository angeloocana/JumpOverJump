angular.module("jojApp").config(['admobSvcProvider', function (admobSvcProvider) {
    // Optionally you can configure the options here:
    admobSvcProvider.setOptions({
        publisherId: "ca-app-pub-2139742733068358/3678879824"
    });
}]);