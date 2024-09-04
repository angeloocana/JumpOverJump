"use strict";

angular.module("jojApp").directive("sgPagination",
    [function () {
        return {
            templateUrl: "app/SharedKernel/directives/sgPagination/sgPaginationTemplate.html",
            scope: {
                page: "=",
                totalPages: "="
            },
            link: function (scope) {
                scope.hasPreviousPage = function() {
                     return scope.page > 1;
                }
                scope.hasNextPage = function() {
                     return scope.page < scope.totalPages;
                }

                scope.goPreviousPage = function () {
                    scope.goPage(scope.page - 1);
                }

                scope.goNextPage = function () {
                    scope.goPage(scope.page + 1);
                }

                scope.goLastPage = function () {
                    scope.goPage(scope.totalPages);
                }

                scope.goPage = function (page) {
                    scope.$parent.goPage(page);
                }
            }
        }
    }
    ]);