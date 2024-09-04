(function (i, s, o, g, r, a, m) {
    i["GoogleAnalyticsObject"] = r; i[r] = i[r] || function () {
        (i[r].q = i[r].q || []).push(arguments)
    }, i[r].l = 1 * new Date(); a = s.createElement(o),
    m = s.getElementsByTagName(o)[0]; a.async = 1; a.src = g; m.parentNode.insertBefore(a, m)
})(window, document, "script", "//www.google-analytics.com/analytics.js", "ga");

ga("create", "UA-61019439-2", "auto");
ga("require", "linkid");
ga("send", "pageview");

angular.module("jojApp").run(["$rootScope", "$location", function ($rootScope, $location) {
    $rootScope.$on("$routeChangeSuccess", function () {
        ga("send", "pageview", { page: $location.path() });
    });
}]);