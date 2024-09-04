"use strict";

angular.module("jojApp").directive("sgShare", ["$translate", "$location", function ($translate, $location) {
    return {
        templateUrl: "app/Share/widgets/sgShare/sgShareTemplate.html",
        scope: {
            emailOpened: "=",
            emailTitle: "=",
            aligment: "=",
            emailSentCallBack: "=",
            emailSentToastMsg: "=",
            postSendEmail: "=",
            link: "="
        },
        link: function (scope) {
            if (!scope.link)
                scope.link = $location.absUrl();

            scope.openEmail = function () {
                scope.emailOpened = scope.emailOpened ? false : true;
            }

            if (!scope.emailTitle) {
                scope.emailTitle = "DEFAULT_SHARE_BY_EMAIL_TITLE";
            }

            if (!scope.aligment) {
                scope.aligment = "right";
            }

            var isEmailValid = function (email) {
                var regex = /^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$/;
                return regex.test(email);
            }

            var validateEmails = function (emailsNotValidated) {
                var emails = [];
                for (var i = 0; i < emailsNotValidated.length; i++) {
                    var email = emailsNotValidated[i];
                    if (isEmailValid(email))
                        emails.push(email);
                    else 
                        scope.errors = ["ERROR_EMAIL_NOT_VALID"];
                }
                return emails;
            }

            scope.sendEmail = function () {
                var emails = [];
                scope.errors = [];
                if (scope.emailTo && scope.emailTo.emails) {
                    scope.emailTo.emails = scope.emailTo.emails.replace(",", ";");
                    emails = emails.concat(validateEmails(scope.emailTo.emails.split(";")));

                    if (scope.errors && scope.errors.length > 0) {
                        scope.sendingEmail = false;
                        return;
                    }
                }

                if (emails.length === 0) {
                    scope.errors = ["ERROR_NO_EMAIL_INFORMED"];
                    scope.sendingEmail = false;
                    return;
                }

                scope.sendingEmail = true;
                scope.postSendEmail(emails).then(function () {
                    scope.sendingEmail = false;
                    scope.openEmail();

                    if (scope.emailSentCallBack)
                        scope.emailSentCallBack();

                    if (!scope.emailSentToastMsg)
                        scope.emailSentToastMsg = "DEFAULT_SHARED_BY_EMAIL_MSG";

                    $translate(scope.emailSentToastMsg).then(function (msg) {
                        Materialize.toast(msg, 10000);
                    });
                });
            }
        }
    }
}
]);