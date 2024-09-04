angular.module("jojApp")
    .run(["$ionicPlatform", function ($ionicPlatform) {
        $ionicPlatform.ready(function () {
            if (window.cordova && window.cordova.plugins.Keyboard) {
                cordova.plugins.Keyboard.hideKeyboardAccessoryBar(true);
            }
            if (window.StatusBar) {
                StatusBar.styleDefault();
            }

            if (typeof analytics !== 'undefined') {
                analytics.startTrackerWithId("UA-61019439-4");
            } else {
                //alert("Google Analytics Unavailable");
            }
        });
    }])
    .config(["$provide", function ($provide) {
        $provide.decorator("$exceptionHandler", [
            "$delegate", function ($delegate) {
                return function (exception, cause) {
                    $delegate(exception, cause);
                    alert(exception.message);
                }
            }
        ]);
    }])
     .config(["$stateProvider", "$urlRouterProvider",
        function ($stateProvider, $urlRouterProvider) {
            $stateProvider
                .state("game", {
                    abstract: true,
                    url: "/game",
                    templateProvider: function ($templateCache) {
                        return $templateCache.get("app/Game/pages/Game/gameTemplate.html");
                    }
                })
                .state("game.new", {
                    url: "/new",
                    views: {
                        "menuContent": {
                            templateProvider: function ($templateCache) {
                                return $templateCache.get("app/Game/widgets/jojNewGame/jojNewGameTemplate.html");
                            },
                            controller: 'jojNewGameController'
                        }
                    }
                })
                .state("game.play", {
                    url: "/play",
                    cache: false,
                    views: {
                        "menuContent": {
                            templateProvider: function ($templateCache) {
                                return $templateCache.get("app/Game/widgets/jojGame/jojGameTemplate.html");
                            },
                            controller: 'jojGameController'
                        }
                    }
                })
                .state("game.computer", {
                    url: "/computer",
                    views: {
                        "menuContent": {
                            templateProvider: function ($templateCache) {
                                return $templateCache.get("app/Game/widgets/jojComputerPlayer/jojComputerPlayerTemplate.html");
                            },
                            controller: "jojComputerPlayerController"
                        }
                    }
                })
                .state("game.computerVsComputer", {
                    url: "/computerVsComputer",
                    cache:false,
                    views: {
                        "menuContent": {
                            controller: "jojComputerVsComputerController"
                        }
                    }
                })
                .state("game.offline", {
                    url: "/offline",
                    views: {
                        "menuContent": {
                            templateProvider: function ($templateCache) {
                                return $templateCache.get("app/Game/widgets/jojOfflinePlayers/jojOfflinePlayersTemplate.html");
                            },
                            controller: "jojOfflinePlayersController"
                        }
                    }
                })
                .state("game.signin", {
                    url: "/signin",
                    views: {
                        "menuContent": {
                            templateProvider: function ($templateCache) {
                                return $templateCache.get("app/Users/pages/SignIn/SignInTemplate.html");
                            }
                        }
                    }
                })
                .state("game.language", {
                    url: "/language",
                    views: {
                        "menuContent": {
                            templateProvider: function ($templateCache) {
                                return $templateCache.get("app/Languages/pages/languagesTemplate.html");
                            }
                        }
                    }
                })
                .state("app", {
                    url: "/app",
                    templateProvider: function ($templateCache) {
                        return $templateCache.get("app/SharedKernel/widgets/ptzMenu/ptzMenuTemplate.html");
                    }
                });

            $urlRouterProvider.otherwise("/game/new");
        }]);