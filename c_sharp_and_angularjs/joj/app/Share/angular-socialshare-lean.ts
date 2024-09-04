"use strict";

angular.module("jojApp")
	.factory("$FB", ["$window", function($window) {
		return {
			init: function(fbId) {
				if (fbId) {
					this.fbId = fbId;
					$window.fbAsyncInit = function() {
						FB.init({
							appId: fbId,
							version    : 'v2.5',
							xfbml: true
						});
					};
					(function (d, s, id) {
					    var js, fjs = d.getElementsByTagName(s)[0];
					    if (d.getElementById(id)) { return; }
					    js = d.createElement(s); js.id = id;
					    js.src = "//connect.facebook.net/en_US/sdk.js";
					    fjs.parentNode.insertBefore(js, fjs);
					}(document, 'script', 'facebook-jssdk'));
				} else {
					throw ("FB App Id Cannot be blank");
				}
			}
		};

	}]).directive("facebook", ["$http", function($http) {
		return {
			scope: {
				shares: "="
			},
			template:"<i class=\"ico-facebook\"></i>",
			transclude: true,
			link: function(scope, element, attr) {
				attr.$observe("url", function() {
					element.unbind();
					element.bind("click", function(e) {
						FB.ui({
							method: "share",
							href: attr.url
						});
						e.preventDefault();
					});
				});
			}
		};
	}]).directive("linkedin", ["$timeout", "$http", "$window", function($timeout, $http, $window) {
		return {
			scope: {
				shares: "="
			},
			transclude: true,
			template: "<i class='ico-linkedin'></i>",
			link: function(scope, element, attr) {
				var renderLinkedinButton = debounce(function() {
					$timeout(function() {
						element.unbind();
						element.bind("click", function() {
						    var url = encodeURIComponent(attr.url).replace(/'/g, "%27").replace(/"/g, "%22");
							$window.open("//www.linkedin.com/shareArticle?mini=true&url=" + url + "&title=" + attr.title + "&summary=" + attr.summary);
						});
					});
				}, 100);
				attr.$observe("url", renderLinkedinButton);
				attr.$observe("title", renderLinkedinButton);
				attr.$observe("summary", renderLinkedinButton);
			}
		};
	}]);
//Simple Debounce Implementation
//http://davidwalsh.name/javascript-debounce-function
function debounce(func, wait, immediate) {
	var timeout;
	return function() {
		var context = this,
			args = arguments;
		var later = function() {
			timeout = null;
			if (!immediate) func.apply(context, args);
		};
		var callNow = immediate && !timeout;
		clearTimeout(timeout);
		timeout = setTimeout(later, wait);
		if (callNow) func.apply(context, args);
	};
};
//http://stackoverflow.com/questions/1349404/generate-a-string-of-5-random-characters-in-javascript
/**
 * RANDOM STRING GENERATOR
 *
 * Info:      http://stackoverflow.com/a/27872144/383904
 * Use:       randomString(length [,"A"] [,"N"] );
 * Default:   return a random alpha-numeric string
 * Arguments: If you use the optional "A", "N" flags:
 *            "A" (Alpha flag)   return random a-Z string
 *            "N" (Numeric flag) return random 0-9 string
 */
function randomString(len, an) {
	an = an && an.toLowerCase();
	var str = "",
		i = 0,
		min = an == "a" ? 10 : 0,
		max = an == "n" ? 10 : 62;
	for (; i++ < len;) {
		var r = Math.random() * (max - min) + min << 0;
		str += String.fromCharCode(r += r > 9 ? r < 36 ? 55 : 61 : 48);
	}
	return str;
}
