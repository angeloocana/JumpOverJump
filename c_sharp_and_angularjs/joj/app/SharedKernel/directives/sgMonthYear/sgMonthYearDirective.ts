"use strict";

angular.module("jojApp").directive("sgMonthYear",
    ["$log", function ($log) {
        return {
            templateUrl: "app/SharedKernel/directives/sgMonthYear/sgMonthYearTemplate.html",
            scope: {
                required: "=",
                monthYear: "=",
                startMonthYear: "=",
                endMonthYear: "=",
                endToday : "="
            },
            link: function (scope) {
                if (!scope.monthYear)
                    scope.monthYear = {};

                if (!scope.startMonthYear)
                    scope.startMonthYear = {
                        month: 1,
                        year:1900
                    };

                if (scope.endToday || !scope.endMonthYear) {
                    var today = new Date();
                    scope.endMonthYear = { year: today.getFullYear(), month: today.getMonth() + 1 }
                }

                var setYears = function () {
                    scope.years = [];
                    for (var year = scope.endMonthYear.year; year >= scope.startMonthYear.year; year--) {
                        scope.years.push(year);
                    }
                }

                var setMonths = function () {
                    scope.months = [];
                    var lastMonth = scope.monthYear.year * 1 === scope.endMonthYear.year * 1 ? scope.endMonthYear.month : 12;
                    var firstMonth = scope.monthYear.year === scope.startMonthYear.year ? scope.startMonthYear.month : 1;
                    for (var month = firstMonth; month <= lastMonth; month++) {
                        scope.months.push(month);
                    }
                }

                scope.yearChanged = function() {
                    setMonths();
                }

                setYears();
                setMonths();
            }
        }
    }
    ]);