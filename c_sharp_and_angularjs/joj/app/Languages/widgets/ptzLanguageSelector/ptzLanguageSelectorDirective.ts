"use strict";

angular.module("jojApp").directive("ptzLanguageSelector",
    ["languageService", "$translate", "$rootScope", "$localStorage", "$log", "$window", "ptzResponsive",
        function (languageService, $translate, $rootScope, $localStorage, $log, $window, ptzResponsive) {
            return {
                templateUrl: "app/Languages/widgets/ptzLanguageSelector/ptzLanguageSelectorTemplate.html",
                link: function (scope, el, attr, ctrl) {

                    if (typeof analytics !== 'undefined') {
                        analytics.trackView("ptzLanguageSelector");
                    }

                    languageService.getLanguages()
                        .then(function (data) {
                            scope.languages = data;
                        });

                    scope.changeLangKey = function (langKey) {
                        $translate.use(langKey);
                        scope.langKey = langKey;
                    };

                    scope.langKey = $localStorage.preferredLanguage;

                    $rootScope.$on("$translateChangeSuccess", function () {
                        $localStorage.preferredLanguage = $translate.use();
                    });

                    var w = angular.element($window);

                    var setLanguagesColClass = function () {
                        scope.languagesColClass = ptzResponsive.returnOnlyFor("", " col-50 col-offset-25 ", " col-33 col-offset-33 ");
                    };

                    w.bind("resize", function () {
                        setLanguagesColClass();
                        scope.$apply();
                    });
                    setLanguagesColClass();
                }
            }
        }
    ]);