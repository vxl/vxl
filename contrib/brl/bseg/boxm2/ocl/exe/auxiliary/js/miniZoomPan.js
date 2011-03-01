/*!
 * jQuery miniZoomPan 1.0
 * 2009 Gian Carlo Mingati
 * Version: 1.0 (18-JUNE-2009)
 * Dual licensed under the MIT and GPL licenses:
 * http://www.opensource.org/licenses/mit-license.php
 * http://www.gnu.org/licenses/gpl.html
 *
 * Requires:
 * jQuery v1.3.2 or later
 *
 * Modified by Andrew Miller (hover replaced by double click)
 */
var zoomed = false;
var zoomLevel = 0;
jQuery.fn.miniZoomPan = function(settings) {
  settings = jQuery.extend({
    sW: 10, // small image width
    sH: 10, // small image height
    lW: 20, // large image width
    lH: 20, // large image height
    levels: [1, 1.5, 2.0, 2.5, 3.0],
    frameColor: "#cecece",
    frameWidth: 1,
    loaderContent: "loading..." // plain text or an image tag eg.: "<img src='yoursite.com/spinner.gif' />"
  }, settings);

  return this.each(function() {
    var div =  jQuery(this);
    div.css({width: settings.sW, height: settings.sH, border: settings.frameWidth+"px solid "+settings.frameColor}).addClass("minizoompan");
    var ig = div.children();
    ig.css({position: "relative"});
    jQuery(window).bind("load", function() {
      ig.width(settings.sW);
      ig.height(settings.sH);
    });
    div.css({overflow: "hidden"});
    //jQuery("<span class='loader'>"+settings.loaderContent+"<\/span>").insertBefore(ig);

    div.mousewheel(function(e, delta) {
      if (delta > 0)
        zoomIn(e);
      else if (delta < 0)
        zoomOut(e);
      return false; // prevent default
    });

    div.dblclick(
      function (e) {
        if (!zoomed)
          zoomIn(e);
        else
          zoomOut(e);
      }
    );

    //Zoom In increments the zoomLevel
    function zoomIn(e) {
      //make sure zoomLevel doesn't go too high
      zoomLevel = (zoomLevel < settings.levels.length-1) ? zoomLevel+1 : zoomLevel;

      //ig.css({ width: settings.lW, height: settings.lH});
      var factor = settings.levels[zoomLevel];
      ig.css({ width: factor*settings.sW, height: factor*settings.sH});
      var hiSrc = ig.attr("src").replace(/s.jpg/, 'l.jpg');
      swapImage(ig, hiSrc);
      div.children("span.loader").fadeIn(250);
      zoomed = true;

      //line up the center of the div to where the mouse location is
      var divWidth = div.width();
      var divHeight = div.height();
      var igW = ig.width();
      var igH = ig.height();
      var dOs = div.offset();
      var leftPan = (e.pageX - dOs.left) * (divWidth - igW) / (divWidth+settings.frameWidth*2);
      var topPan = (e.pageY - dOs.top) * (divHeight - igH) / (divHeight+settings.frameWidth*2);
      ig.css({left: leftPan, top: topPan});
    }

    function zoomOut(e) {
      zoomLevel = (zoomLevel <= 0) ? 0 : zoomLevel-1;
      var factor = settings.levels[zoomLevel];
      if (zoomLevel == 0)
        ig.css({left: "0", top: "0", width: settings.sW, height: settings.sH});
      else
        ig.css({ width: factor*settings.sW, height: factor*settings.sH});
      var loSrc = ig.attr("src").replace(/l.jpg/, 's.jpg');
      swapImage(ig, loSrc);
      div.children("span.loader").fadeOut(250);
      zoomed = false;

      var divWidth = div.width();
      var divHeight = div.height();
      var igW = ig.width();
      var igH = ig.height();
      var dOs = div.offset();
      var leftPan = (e.pageX - dOs.left) * (divWidth - igW) / (divWidth+settings.frameWidth*2);
      var topPan = (e.pageY - dOs.top) * (divHeight - igH) / (divHeight+settings.frameWidth*2);
      ig.css({left: leftPan, top: topPan});
    }

    function swapImage(param, uri){
      param.load(function () {
        div.children("span.loader").fadeOut(250);
      }).error(function (){
      alert("Image does not exist or its SRC is not correct.");
      }).attr('src', uri);
    }
  });
};
