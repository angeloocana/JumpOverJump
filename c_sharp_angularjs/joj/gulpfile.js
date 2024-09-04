var gulp = require("gulp");
var concat = require("gulp-concat");
var angularFileSort = require("gulp-angular-filesort");
var strip = require("gulp-strip-line");
var templateCache = require("gulp-angular-templatecache");
var minify = require('gulp-minify');
var runSequence = require('run-sequence');
var ngAnnotate = require('gulp-ng-annotate');
var cssnano = require('gulp-cssnano');
var htmlmin = require('gulp-htmlmin');
var sass = require('gulp-sass');

var dist = "./dist/";
var distIonic = "../jojApp/www/dist/";
var distWeb = "../JumpOverJump.StaticContent/dist/";

var exportDist = function (stream) {
    stream
    .pipe(gulp.dest(distIonic))
    .pipe(gulp.dest(distWeb));
}

var distCss = "style-0.0.1.min.css";

var css = function () {
    exportDist(gulp.src(['./lib/ionic/css/ionic.css',
        "./css/_vars.scss",
        "./css/_mixings.scss",
        "./css/_extends.scss",
        "!/css/**/_*.scss", "./css/**/*.scss", "./app/**/*.scss"])
   .pipe(concat(distCss))
        .pipe(sass().on('error', sass.logError))
   .pipe(cssnano())
   .pipe(gulp.dest(dist)));
}

gulp.task("css", css);

var compress = function (stream) {
    exportDist(
        stream
        .pipe(minify({
            exclude: ['tasks'],
            ignoreFiles: ['.combo.js', '-min.js']
        }))
        .pipe(gulp.dest(dist))
    );
};

var distJsWeb = "app-0.1.0.web.js";
var distJsMobile = "app-0.1.0.mobile.js";

var js = function () {
    compress(
        gulp.src(["app/**/*.ts", "!app/**/*.web.ts"])
        .pipe(angularFileSort())
        .pipe(strip(["use strict"]))
        .pipe(concat(distJsMobile))
        .pipe(ngAnnotate())
        .pipe(gulp.dest(dist))
    );

    compress(
       gulp.src(["app/**/*.ts", "!app/**/*.mobile.ts"])
       .pipe(angularFileSort())
       .pipe(strip(["use strict"]))
       .pipe(concat(distJsWeb))
       .pipe(ngAnnotate())
       .pipe(gulp.dest(dist))
   );
}

var html = function () {
    gulp.src(["./app/**/*.html"])
        .pipe(htmlmin({ collapseWhitespace: true }))
    .pipe(templateCache({
        root: "app/",
        module: "jojApp"
    }))
    .pipe(gulp.dest("./app/"));
    //.pipe(js());
}

gulp.task("js", js);
gulp.task("jsHtml", ["buildHtml"], js);

gulp.task("buildHtml", html);

gulp.task("html", ['buildHtml', 'jsHtml']);

var distExtJs = "ext-modules-0.0.1.js";

var buildExternalModules = function () {
    compress(gulp
       .src(['./lib/ionic/js/ionic.bundle.js',
              './lib/ngStorage.min.js',
              './lib/angular-translate.js'
       ])
   .pipe(concat(distExtJs))
   .pipe(gulp.dest(dist)));
}

gulp.task("xt", buildExternalModules);

var publish = function () {
    runSequence('buildExternalModules', 'builTemplateCache', 'buildJavaScript', 'buildCss', 'compress', 'copy');
};

gulp.task('default', publish);

gulp.task('watch', function (cb) {
    gulp.watch('./app/**/*.js').on('change', function (file) {
        publish();
    });

    gulp.watch('./app/**/*.html').on('change', function (file) {
        publish();
    });
});


var rename = require("gulp-rename");

gulp.task('rename', function(){
    gulp.src("./app/**/*.js")
    .pipe(rename(function (path) {
        path.extname = ".ts";
        return path;
    }))
    .pipe(gulp.dest("./app"));    
});

var clean = require('gulp-clean');
 
gulp.task('clean-scripts', function () {
  return gulp.src("./app/**/*.js", {read: false})
    .pipe(clean());
});