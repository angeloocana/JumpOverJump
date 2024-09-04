"use strict";

angular.module("jojApp").directive("jojPlayer",
    ["$log",
        function ($log) {
            return {
                templateUrl: "app/Game/widgets/jojPlayer/jojPlayerTemplate.html",
                scope: {
                    player: "=",
                    runOnHelpOnChanged: "=",
                    isBlack:"="
                },
                link: function (scope, element) {
                    scope.$watch('player.helpOn', function (newValue, oldValue) {
                        if (scope.runOnHelpOnChanged)
                            scope.runOnHelpOnChanged();
                    });
                }
            }
        }
    ]);