angular.module("jojApp").run(["$templateCache", function($templateCache) {$templateCache.put("app/Languages/pages/languagesTemplate.html","<ion-view view-title=\"{{\'LANGUAGE\' | translate}}\"><ion-content class=\"app-background-color\"><ptz-language-selector></ptz-language-selector></ion-content></ion-view>");
$templateCache.put("app/SharedKernel/pages/homeTemplate.html","<ion-nav-bar class=\"bar-positive\"><h1 class=\"title\">Jump Over Jump</h1></ion-nav-bar><ion-tabs class=\"tabs-positive tabs-icon-top\"><ion-tab title=\"{{ \'NEW_GAME_OFFLINE\' | translate }}\" icon=\"ion-home\" ng-click=\"newGameOffline()\"><ion-view></ion-view></ion-tab><ion-tab title=\"{{ \'NEW_GAME_VS_COMPUTER\' | translate }}\" icon=\"ion-home\" ng-click=\"newGameVsComputer()\"><ion-view></ion-view></ion-tab><ion-tab title=\"{{ \'NEW_GAME_ONLINE\' | translate }}\" icon=\"ion-home\" ng-click=\"newGameOnline()\"><ion-view></ion-view></ion-tab><ion-tab title=\"{{ \'BACK_MOVE\' | translate }}\" icon=\"ion-home\" ng-click=\"backMove()\"><ion-view></ion-view></ion-tab></ion-tabs>");
$templateCache.put("app/Languages/widgets/ptzLanguageSelector/ptzLanguageSelectorTemplate.html","<div class=\"row\"><div class=\"col {{languagesColClass}}\"><ion-list><ion-radio ng-model=\"langKey\" ng-click=\"$parent.changeLangKey(language.key)\" ng-value=\"language.key\" ng-repeat=\"language in languages\"><i class=\"country-flag {{language.icon}}\"></i> {{language.name}}</ion-radio></ion-list></div></div>");
$templateCache.put("app/Game/pages/Game/gameTemplate.html","<ion-side-menus enable-menu-with-back-views=\"true\"><ion-side-menu-content><ion-nav-bar class=\"bar-dark\"><ion-nav-back-button></ion-nav-back-button><ion-nav-buttons side=\"right\"><button class=\"button button-icon button-clear ion-navicon\" menu-toggle=\"right\"></button></ion-nav-buttons></ion-nav-bar><ion-nav-view name=\"menuContent\"></ion-nav-view></ion-side-menu-content><ion-side-menu side=\"right\"><ion-header-bar class=\"bar-dark\"><h1 class=\"title\">Jump Over Jump</h1></ion-header-bar><ion-content><ion-list><ion-item menu-close ui-sref=\"game.new\" class=\"item-icon-left\"><i class=\"icon ion-ios-game-controller-b\"></i> {{\"NEW_GAME\" | translate}}</ion-item><ion-item menu-close ui-sref=\"game.language\" class=\"item-icon-left\"><i class=\"icon ion-android-globe\"></i> {{\"LANGUAGE\"| translate}}</ion-item><ion-item menu-close href=\"https://www.facebook.com/jojgame\" target=\"_system\" class=\"item-icon-left\"><i class=\"icon ion-social-youtube\"></i> Youtube</ion-item><a ion-item menu-close ptz-href=\"https://www.facebook.com/jojgame\" class=\"item-icon-left\"><i class=\"icon ion-social-facebook\"></i> Facebook</a><ion-item menu-close href=\"https://www.facebook.com/jojgame\" target=\"_system\" class=\"item-icon-left\"><i class=\"icon ion-social-twitter\"></i> Twitter</ion-item></ion-list></ion-content></ion-side-menu></ion-side-menus>");
$templateCache.put("app/Game/widgets/jojBoard/jojBoardTemplate.html","<div class=\"joj-game\"><div class=\"joj-board\" style=\"width: {{boardSize}}px; height: {{boardSize}}px\"><div ng-repeat=\"row in game.board track by $index\" class=\"joj-board-row\"><div ng-repeat=\"col in row track by $index\" class=\"joj-board-col {{col.backgroundBlack ? \'joj-board-black\':\'joj-board-white\'}}\" ng-class=\"{\'joj-position-i-can-go-here\': (helpOn && col.iCanGoHere),\r\n                 \'joj-position-white-home\': col.whiteHome,\r\n                 \'joj-position-black-home\': col.blackHome,\r\n                 \'joj-position-winning\': col.winning,\r\n                 \'joj-position-current\': (col.x == currentPiece.x && col.y == currentPiece.y),\r\n                 \'joj-position-last-move\': col.lastMove,\r\n                 \'joj-position-last-move-jump\': col.lastMoveJump,\r\n                 }\" ng-click=\"move(col)\"><div class=\"{{getPieceClass(col.piece)}}\" ng-click=\"whereCanIGo(col)\"></div></div></div></div><div class=\"joj-menu\" style=\"{{\'width:\'+menuWidth +\'px; height:\'+menuHeigth}}\"><div class=\"row responsive-sm\"><div class=\"col joj-col\"><a class=\"button button-block icon-left button-balanced button-large ion-ios-game-controller-b joj-col-btn\" ui-sref=\"game.new\">{{ \"NEW_GAME\" | translate }}</a></div><div class=\"col joj-col\"><a class=\"button button-block icon-left button-assertive button-large ion-skip-backward joj-col-btn\" ng-click=\"backMove()\">{{ \"BACK_MOVE\" | translate }}</a></div></div><div class=\"row responsive-sm\"><div class=\"col joj-col\"><joj-player player=\"game.players.white\" run-on-help-on-changed=\"helpOnChanged\"></joj-player></div><div class=\"col joj-col\"><joj-player player=\"game.players.black\" is-black=\"true\" run-on-help-on-changed=\"helpOnChanged\"></joj-player></div></div></div></div>");
$templateCache.put("app/Game/widgets/jojComputerPlayer/jojComputerPlayerTemplate.html","<ion-view view-title=\"{{\'ENTER_PLAYER_NAMES\' | translate}}\"><ion-content class=\"app-background-color\"><div class=\"row responsive-sm\"><div class=\"col {{firstCol}} joj-col\"><a class=\"button icon-left button-block button-balanced button-large ion-ios-game-controller-b joj-col-btn\" ng-click=\"play()\">{{ \"PLAY\" | translate }}</a></div><div class=\"col {{secondCol}} joj-col\"><a class=\"button icon-left button-block ion-arrow-swap button-assertive button-large joj-col-btn\" ng-click=\"switchPlayers()\">{{ \"SWITCH_PLAYERS\" | translate }}</a></div></div><div class=\"row responsive-sm\"><div class=\"col col-bottom {{firstCol}} joj-col\"><div class=\"list joj-enter-player-name-list no-margin\" ng-show=\"!players.computerIsWhite\"><label class=\"item item-input item-stacked-label\"><span class=\"input-label\" translate=\"WHITE_PLAYER_NAME\"></span> <input type=\"text\" placeholder=\"{{\'WHITE\' | translate}}\" ng-model=\"players.white.name\"></label></div><joj-player player=\"players.white\" run-on-help-on-changed=\"helpOnChanged\"></joj-player></div><div class=\"col col-bottom {{secondCol}} joj-col\"><div class=\"list joj-enter-player-name-list no-margin joj-black-player\" ng-show=\"players.computerIsWhite\"><label class=\"item item-input item-stacked-label joj-black-player\"><span class=\"input-label joj-black-player\" translate=\"BLACK_PLAYER_NAME\"></span> <input type=\"text\" placeholder=\"{{\'BLACK\' | translate}}\" ng-model=\"players.black.name\"></label></div><joj-player player=\"players.black\" is-black=\"true\" run-on-help-on-changed=\"helpOnChanged\"></joj-player></div></div></ion-content></ion-view>");
$templateCache.put("app/Game/widgets/jojGame/jojGameTemplate.html","<ion-view view-title=\"Jump Over Jump\"><ion-content class=\"app-background-color\"><joj-board></joj-board></ion-content></ion-view>");
$templateCache.put("app/Game/widgets/jojNewGame/jojNewGameTemplate.html","<ion-view view-title=\"{{\'NEW_GAME\' | translate}}\"><ion-content class=\"app-background-color\"><div class=\"row\"><div class=\"col {{btnsContaneirClass}}\"><a class=\"button icon-left button-block button-balanced button-large ion-ios-game-controller-b\" ui-sref=\"game.offline\">{{ \"NEW_GAME_OFFLINE\" | translate }}</a> <a class=\"button icon-left button-block button-balanced button-large ion-android-desktop\" ui-sref=\"game.computer\">{{ \"NEW_GAME_VS_COMPUTER\" | translate }}</a><!--<a class=\"button icon-left button-block button-balanced button-large ion-android-contacts\" ui-sref=\"game.online\">\r\n                    {{ \"NEW_GAME_ONLINE\" | translate }}\r\n                </a>--></div></div><ptz-footer></ptz-footer></ion-content></ion-view>");
$templateCache.put("app/Game/widgets/jojOfflinePlayers/jojOfflinePlayersTemplate.html","<ion-view view-title=\"{{\'ENTER_PLAYER_NAMES\' | translate}}\"><ion-content class=\"app-background-color\"><div class=\"row responsive-sm\"><div class=\"col {{firstCol}} joj-col\"><a class=\"button icon-left button-block button-balanced button-large ion-ios-game-controller-b joj-col-btn\" ng-click=\"play()\">{{ \"PLAY\" | translate }}</a></div><div class=\"col {{secondCol}} joj-col\"><a class=\"button icon-left button-block ion-arrow-swap button-assertive button-large joj-col-btn\" ng-click=\"switchPlayers()\">{{ \"SWITCH_PLAYERS\" | translate }}</a></div></div><div class=\"row responsive-sm\"><div class=\"col {{firstCol}} joj-col\"><div class=\"list joj-enter-player-name-list no-margin\"><label class=\"item item-input item-stacked-label\"><span class=\"input-label\" translate=\"WHITE_PLAYER_NAME\"></span> <input type=\"text\" placeholder=\"{{\'WHITE\' | translate}}\" ng-model=\"players.white.name\"></label></div><joj-player player=\"players.white\" run-on-help-on-changed=\"helpOnChanged\"></joj-player></div><div class=\"col {{secondCol}} joj-col\"><div class=\"list joj-enter-player-name-list joj-black-player no-margin\"><label class=\"item item-input item-stacked-label joj-black-player\"><span class=\"input-label joj-black-player\" translate=\"BLACK_PLAYER_NAME\"></span> <input type=\"text\" placeholder=\"{{\'BLACK\' | translate}}\" ng-model=\"players.black.name\"></label></div><joj-player player=\"players.black\" is-black=\"true\" run-on-help-on-changed=\"helpOnChanged\"></joj-player></div></div></ion-content></ion-view>");
$templateCache.put("app/Game/widgets/jojPlayer/jojPlayerTemplate.html","<div class=\"list card no-margin\" ng-class=\"{\'joj-black-player\': isBlack }\"><div class=\"item item-avatar item-icon-right\" ng-class=\"{\'joj-black-player\': isBlack }\"><img ng-src=\"{{player.foto}}\" alt=\"{{player.name}}\"><h2 ng-class=\"{\'joj-black-player\': isBlack }\">{{player.name}}</h2><p ng-class=\"{\'joj-black-player\': isBlack }\">{{player.color | translate}}</p><i ng-class=\"{\'joj-black-player\': isBlack }\" class=\"icon {{player.icon}}\"></i></div><div class=\"item item-body\" ng-class=\"{\'joj-black-player\': isBlack }\"><ul class=\"list\"><li class=\"item item-toggle item-icon-left\" ng-class=\"{\'joj-black-player\': isBlack }\"><i ng-class=\"{\'joj-black-player\': isBlack }\" class=\"icon {{player.helpOn ? \'ion-eye\' : \'ion-eye-disabled\'}}\"></i> {{\"SHOW_MOVES\" | translate}}<label class=\"toggle toggle-balanced\" ng-class=\"{\'joj-black-player\': isBlack }\"><input ng-model=\"player.helpOn\" type=\"checkbox\" ng-disabled=\"player.isComputer\"><div class=\"track\"><div class=\"handle\"></div></div></label></li></ul></div></div>");
$templateCache.put("app/Share/widgets/sgShare/sgShareTemplate.html","<div class=\"col s12 valign-wrapper {{aligment}}\"><span class=\"{{aligment}} valign text-white\">{{\"SHARE\" | translate}}:</span> <a class=\"btn-sm-shared facebook {{aligment}}\" title=\"Facebook\" data-shares=\"shares\" facebook data-url=\"{{link}}\"><i class=\"ico-facebook\"></i></a> <a class=\"btn-sm-shared linkedin {{aligment}}\" title=\"linkedin\" linkedin data-url=\"{{link}}\"></a> <a class=\"btn-sm-shared gplus {{aligment}}\" title=\"Google+\" ng-href=\"https://plus.google.com/share?url={{link}}\" onclick=\"window.open(this.href,\r\n  \'\', \'menubar=no,toolbar=no,resizable=yes,scrollbars=yes,height=600,width=600\');return false\"><i class=\"ico-gplus\"></i></a> <a class=\"btn-sm-shared email {{aligment}}\" ng-show=\"postSendEmail\" ng-click=\"openEmail()\" title=\"E-mail\"><i class=\"material-icons\">email</i></a></div><div ng-show=\"emailOpened\"><form id=\"emailToForm\" name=\"emailToForm\"><div class=\"col s12 blue-text {{aligment}}\"><h5>{{emailTitle | translate}}</h5></div><div class=\"input-field col s12\"><input id=\"share-email\" type=\"text\" class=\"validate\" minlength=\"5\" maxlength=\"500\" ng-model=\"emailTo.emails\"><label for=\"share-email\" translate=\"EMAILS_TO\"></label></div><div class=\"col s12 {{aligment}}\"><sg-errors></sg-errors><sg-loader is-loading=\"sendingEmail\"></sg-loader><button type=\"submit\" class=\"blue-sg waves-effect waves-light click {{aligment}} no-margin-top\" ng-click=\"sendEmail()\" translate=\"SHARE\" ng-hide=\"sendingEmail\"></button></div></form></div>");
$templateCache.put("app/SharedKernel/directives/sgAction/sgActionTemplate.html","<button type=\"submit\" class=\"blue-sg waves-effect waves-light\" ng-disabled=\"actionForm.$invalid\" ng-click=\"startAction()\" ng-hide=\"isLoading\">{{actionLabel | translate}}</button><sg-errors errors=\"errors\"></sg-errors><sg-loader is-loading=\"isLoading\"></sg-loader>");
$templateCache.put("app/SharedKernel/directives/sgBreadcrumbs/sgBreadcrumbsTemplate.html","<div class=\"col s12\"><ol class=\"breadcrumb\"><li ng-repeat=\"page in pages\" ng-class=\"{\'active\': page.active}\"><h5 ng-if=\"!(page.link) && !(page.active)\" class=\"breadcrumbs-title blue-text\">{{ page.label | translate}}</h5><a ng-if=\"page.link && !(page.active)\" ng-href=\"{{page.link}}\">{{ page.label | translate}}</a> {{ page.active ? (page.label | translate) : \'\'}}</li></ol></div>");
$templateCache.put("app/SharedKernel/directives/sgLoader/sgLoaderTemplate.html","<div class=\"preloader-wrapper big active\" ng-show=\"isLoading\"><div class=\"spinner-layer spinner-blue-only\"><div class=\"circle-clipper left\"><div class=\"circle\"></div></div></div></div>");
$templateCache.put("app/SharedKernel/directives/sgMonthYear/sgMonthYearTemplate.html","<div class=\"input-field-select col s12 m6\"><label translate=\"YEAR\"></label><select class=\"browser-default validate\" ng-required=\"required\" ng-model=\"monthYear.year\" ng-change=\"yearChanged()\" ng-options=\"year as year for year in years\"><option value=\"\" translate=\"SELECT\"></option></select></div><div class=\"input-field-select col s12 m6\"><label translate=\"MONTH\"></label><select class=\"browser-default validate\" ng-required=\"required\" ng-model=\"monthYear.month\" convert-to-number ng-options=\"month as (month | month | translate) for month in months\"><option value=\"\" translate=\"SELECT\"></option></select></div>");
$templateCache.put("app/SharedKernel/directives/sgPagination/sgPaginationTemplate.html","<div class=\"col s12 m1 valign-wrapper\"><div class=\"col s12 valign\"><div class=\"valign-wrapper\"><a ng-show=\"hasPreviousPage()\" ng-click=\"goPreviousPage()\" class=\"valign\"><i class=\"material-icons\">chevron_left</i></a> <b class=\"valign\">{{page}}</b> <a>&nbsp;/&nbsp;</a> <a ng-click=\"goLastPage()\" class=\"valign\">{{totalPages}}</a> <a ng-click=\"goNextPage()\" ng-show=\"hasNextPage()\" class=\"valign\"><i class=\"material-icons valign\">chevron_right</i></a></div></div></div>");
$templateCache.put("app/SharedKernel/widgets/ptzFooter/ptzFooterTemplate.html","<div class=\"row\"><div class=\"col logo-container\"><div class=\"logo\"></div></div></div><div class=\"row\"><div class=\"col center\"><a href=\"http://jojsc.azurewebsites.net/android.apk\" class=\"center\"><i class=\"icon ion-social-android icon-larger center dark\"></i></a></div></div>");
$templateCache.put("app/SharedKernel/widgets/ptzMenu/ptzMenuTemplate.html","<ion-side-menus><ion-side-menu-content><ion-nav-bar class=\"bar-calm\"><ion-nav-back-button></ion-nav-back-button><ion-nav-buttons side=\"left\"><button class=\"button button-icon button-clear ion-navicon\" menu-toggle=\"left\"></button></ion-nav-buttons></ion-nav-bar><ion-nav-view name=\"menuContent\"></ion-nav-view></ion-side-menu-content><ion-side-menu side=\"left\"><ion-header-bar class=\"bar-calm\"><h1 class=\"title\">Menu</h1></ion-header-bar><ion-content class=\"has-header\"><ion-list><ion-item nav-clear menu-close ng-click=\"login()\">Login</ion-item><ion-item nav-clear menu-close href=\"#/app/search\">Game</ion-item></ion-list></ion-content></ion-side-menu></ion-side-menus>");
$templateCache.put("app/SharedKernel/widgets/sgErrors/sgErrorsTemplate.html","<ul class=\"red-text\"><li ng-repeat=\"error in errors\">{{error | translate}}</li></ul>");
$templateCache.put("app/SharedKernel/pages/aboutUs/aboutUsTemplate.html","<div id=\"index-banner\" class=\"parallax-container\"><div class=\"parallax cover\"><img src=\"http://staffgeistsc.azurewebsites.net/img/cover-staffgeist.jpg\" alt=\"Staffgeist\"></div></div><div class=\"container\"><div class=\"section\"><div class=\"col s12 center\"><h3 class=\"breadcrumbs-title title-about\">Staffgeist</h3><h5 class=\"center\"><i>Avalie a sua empresa, compartilhe a sua opinião!</i></h5></div></div></div><div class=\"container\"><div class=\"row\"><div class=\"col s12 m12 l10 offset-l1\"><div class=\"col s12 m10 l8 offset-m1 offset-l2\"><h5 class=\"center\">Staffgeist tem a missão de apresentar as empresas mais bem avaliadas por seus colaboradores e ex colaboradores.</h5><div class=\"col s12\"><div class=\"col s12 m1 l1\"><img src=\"http://staffgeistsc.azurewebsites.net/img/icons/build-company2.svg\" alt=\"\" class=\"ico-custom\"></div><div class=\"col s12 m10 l10 offset-l1\"><p>Para as empresas é fundamental entender o seu clima organizacional em tempo real.</p></div></div><div class=\"col s12\"><div class=\"col s12 m1 l1\"><img src=\"http://staffgeistsc.azurewebsites.net/img/icons/business-man.svg\" alt=\"\" class=\"ico-custom\"></div><div class=\"col s12 m10 l10 offset-l1\"><p>Para os colaboradores é gratificante compartilhar as suas conquistas profissionais em tempo real.</p></div></div><div class=\"col s12\"><div class=\"col s12 m1 l1\"><img src=\"http://staffgeistsc.azurewebsites.net/img/icons/business-people.svg\" alt=\"\" class=\"ico-custom\"></div><div class=\"col s12 m10 l10 offset-l1\"><p>Para os futuros colaboradores é fundamental saber se as suas expectativas estão alinhadas com os valores da companhia em que deseja trabalhar.</p></div></div><div class=\"col s12 m6 l10 offset-m3 offset-l1 intro-about-text center\"><div class=\"container\"><div class=\"row\"><!--<h5 class=\"light\">16.685 avaliações - 3.256 empresas cadastradas </h5>--><sg-share></sg-share></div></div></div></div></div></div></div>");
$templateCache.put("app/Users/pages/resetPassword/resetPasswordTemplate.html","<div class=\"container\"><div class=\"row\"><sg-breadcrumbs pages=\"[{label:\'RESET_PASSWORD\'}]\"></sg-breadcrumbs><sg-reset-password id=\"id\" token=\"token\"></sg-reset-password></div></div>");
$templateCache.put("app/Users/pages/SignIn/SignInTemplate.html","<ion-view view-title=\"{{\'LANGUAGE\' | translate}}\"><ion-content class=\"app-background-color\"><ptz-external-login-links></ptz-external-login-links><ptz-sign-in></ptz-sign-in><ptz-create-account></ptz-create-account></ion-content></ion-view>");
$templateCache.put("app/Users/widgets/ptzExternalLoginLinks/ptzExternalLoginLinksTemplate.html","<div class=\"row\"><div class=\"col {{\'SIGN_IN\' | translate}}\"><p>{{\"SIGN_IN_USE_SOCIAL_NETWORKS\" | translate}}</p><div><a target=\"_self\" ng-href=\"login/ExternalLogin?provider=Facebook&returnUrl={{returnUrl}}\"><i class=\"icon ion-social-facebook\"></i></a> <a target=\"_self\" ng-href=\"login/ExternalLogin?provider=Google&returnUrl={{returnUrl}}\"><i class=\"icon ion-social-googleplus\"></i></a></div></div></div>");
$templateCache.put("app/Users/widgets/sgCreateAccount/sgCreateAccountLinkTemplate.html","<ng-switch on=\"showNotRegistred\"><span ng-switch-when=\"true\">{{ \"NOT_REGISTERED\" | translate}}</span> <a ng-switch-when=\"true\" class=\"btn waves-effect white-sg\" ng-class=\"getValidateConfirmPasswordClass()\" ng-href=\"/signIn?returnUrl={{getReturnUrl()}}\" translate=\"CREATE_ACCOUNT\"></a> <a ng-switch-default class=\"blue-text\" ng-class=\"getValidateConfirmPasswordClass()\" ng-href=\"/signIn?returnUrl={{getReturnUrl()}}\" translate=\"CREATE_ACCOUNT\"></a></ng-switch>");
$templateCache.put("app/Users/widgets/sgCreateAccount/sgCreateAccountTemplate.html","<div class=\"col s12\"><h4 translate=\"CREATE_ACCOUNT_TITLE\"></h4></div><form name=\"createAccountForm\" id=\"createAccountForm\"><div class=\"input-field col s12\"><input id=\"name\" type=\"text\" class=\"validate\" placeholder=\"{{\'NAME\' | translate}}\" ng-model=\"newUser.name\" required minlength=\"3\" maxlength=\"256\"><label for=\"name\" translate=\"NAME\"></label></div><div class=\"input-field col s12\"><input id=\"email\" type=\"email\" class=\"validate\" placeholder=\"E-mail\" ng-model=\"newUser.email\" required><label for=\"email\">E-mail</label></div><div class=\"input-field col s12\"><input id=\"password\" type=\"password\" class=\"validate\" placeholder=\"{{\'PASSWORD\' | translate}}\" ng-model=\"newUser.password\" required minlength=\"6\" maxlength=\"40\"><label for=\"password\" translate=\"PASSWORD\"></label></div><div class=\"input-field col s12\"><input id=\"confirm_password\" type=\"password\" class=\"validate\" placeholder=\"{{\'CONFIRM_PASSWORD\' | translate}}\" ng-model=\"newUser.confirmPassword\" required compare-to=\"newUser.password\"><label for=\"confirm_password\" translate=\"CONFIRM_PASSWORD\"></label></div><div class=\"col s12\"><button type=\"submit\" class=\"blue-sg waves-effect waves-light\" ng-disabled=\"createAccountForm.$invalid\" ng-click=\"sendCreateAccountForm()\" translate=\"CREATE_ACCOUNT\" ng-hide=\"isLoading\"></button><sg-errors></sg-errors><sg-loader is-loading=\"isLoading\"></sg-loader></div></form>");
$templateCache.put("app/Users/widgets/sgIForgotMyPassword/sgIForgotMyPasswordTemplate.html","<div class=\"col s12\"><a ng-click=\"openForm()\" translate=\"I_FORGOT_MY_PASSWORD\"></a></div><div ng-show=\"open && !emailSent\"><div class=\"col s12\"><br></div><form name=\"frmSgIForgotMyPassword\" id=\"frmSgIForgotMyPassword\"><div class=\"input-field col s12\"><input id=\"sgIForgotMyPasswordEmail\" type=\"text\" class=\"validate\" placeholder=\"E-mail\" ng-model=\"email\" required><label for=\"sgIForgotMyPasswordEmail\" translate=\"I_FORGOT_MY_PASSWORD_EXPLAIN\"></label></div><div class=\"col s12\"><sg-action errors=\"errors\" action=\"send\" action-form=\"frmSgIForgotMyPassword\" action-label=\"\'SEND\'\"></sg-action></div></form></div><div ng-show=\"emailSent\" class=\"col s12\"><h5 translate=\"I_FORGOT_MY_PASSWORD_SENT_EMAIL\"></h5></div>");
$templateCache.put("app/Users/widgets/sgResetPassword/sgResetPasswordTemplate.html","<div ng-hide=\"passwordChanged\"><form name=\"frmResetPassword\" id=\"frmResetPassword\"><div class=\"input-field col s12 m4 l3\"><input id=\"password\" type=\"password\" class=\"validate\" placeholder=\"{{\'NEW_PASSWORD\' | translate}}\" ng-model=\"resetPasswordModel.newPassword\" required minlength=\"6\" maxlength=\"40\"><label for=\"password\" translate=\"NEW_PASSWORD\"></label></div><div class=\"input-field col s12 m4 l3\"><input id=\"confirm_password\" type=\"password\" class=\"validate\" placeholder=\"{{\'CONFIRM_PASSWORD\' | translate}}\" ng-model=\"resetPasswordModel.confirmPassword\" required compare-to=\"resetPasswordModel.newPassword\"><label for=\"confirm_password\" translate=\"CONFIRM_PASSWORD\"></label></div><div class=\"col s12\"><sg-action errors=\"errors\" action=\"resetPassword\" action-form=\"frmResetPassword\" action-label=\"\'RESET_PASSWORD\'\"></sg-action></div></form></div><div ng-show=\"passwordChanged\" class=\"col s12\"><h5 translate=\"PASSWORD_CHANGED\"></h5><sg-sign-in-link></sg-sign-in-link></div>");
$templateCache.put("app/Users/widgets/sgSignIn/sgSignInLinkTemplate.html","<a class=\"waves-effect\" ng-href=\"/signIn?returnUrl={{getReturnUrl()}}\" translate=\"SIGN_IN\"></a>");
$templateCache.put("app/Users/widgets/sgSignIn/sgSignInTemplate.html","<div class=\"col s12\"><h4 translate=\"SIGN_IN_TITLE\"></h4></div><form name=\"signInForm\" id=\"signInForm\"><div class=\"input-field col s12\"><input id=\"email\" type=\"text\" class=\"validate\" placeholder=\"E-mail\" ng-model=\"user.email\" required><label for=\"email\">E-mail</label></div><div class=\"input-field col s12\"><input id=\"password\" type=\"password\" class=\"validate\" placeholder=\"{{\'PASSWORD\' | translate}}\" ng-model=\"user.password\" required><label for=\"password\" translate=\"PASSWORD\"></label></div><div class=\"col s12\"><button type=\"submit\" ng-disabled=\"signInForm.$invalid\" ng-click=\"sendSingInForm()\" class=\"blue-sg waves-effect waves-light\" translate=\"SIGN_IN\" ng-hide=\"isLoading\"></button><sg-errors></sg-errors><sg-loader is-loading=\"isLoading\"></sg-loader></div></form><sg-i-forgot-my-password></sg-i-forgot-my-password>");
$templateCache.put("app/Users/widgets/sgUserInfo/sgUserInfoTemplate.html","<a href=\"\" class=\"light-blue-text profile-box\"><img ng-src=\"{{userInfo.img}}\" ng-if=\"userInfo.img\" alt=\"\" class=\"circle responsive-img profile-image\"> <span class=\"truncate\">{{userInfo.name}}</span></a>");}]);