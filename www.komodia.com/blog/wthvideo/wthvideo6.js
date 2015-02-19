// JavaScript Document
if (typeof wthvideo == 'undefined') {
	wthvideo = new Object();
}
wthvideo.params = {
	width:320,
	height:320,
	left:"0px",
	right:"auto",
	top:"0px",
	bottom:"auto",
	centeroffset:"auto",
	color:0x022918,                 
	flv:"jedkomodia3099"
};

wthvideo.hideDiv = function(){
	document.getElementById('wthvideo').style.visibility = 'hidden';	
}
function onlyOnce() {
if (document.cookie.indexOf("hasSeen6=true") == -1) {
var later = new Date();
later.setFullYear(later.getFullYear()+10);
document.cookie = 'hasSeen6=true;path=/;';
wthvideo.drawVideo();
}
}
wthvideo.drawVideo= function(){
	var markUp = '';
	markUp += '<style type="text/css">';
	markUp += '#wthvideo {position:fixed;width:'+wthvideo.params.width+'px;height:'+wthvideo.params.height+'px;margin-left:'+wthvideo.params.centeroffset+';left:'+wthvideo.params.left+';right:'+wthvideo.params.right+';top:'+wthvideo.params.top+';bottom:'+wthvideo.params.bottom+';z-index:99999;}';
	markUp += '</style>';
	markUp += '<!--[if lte IE 6]>';
	markUp += '<style type="text/css">';
	markUp += 'html, body{height: 100%;overflow: auto;}#wthvideo {position: absolute;}';
	markUp += '</style>';
	markUp += '<![endif]-->';
	markUp += '<div id="wthvideo">';
	markUp += '  <object id="objvideo" style="outline:none;" type="application/x-shockwave-flash" width="'+wthvideo.params.width+'" height="'+wthvideo.params.height+'" data="wthvideo/wthplayer.swf">';
	markUp += '    <param name="movie" value="wthvideo/wthplayer.swf" />';
	markUp += '    <param name="quality" value="high" />';
	markUp += '    <param name="flashvars" value="vurl='+wthvideo.params.flv+'.flv&amp;vwidth='+wthvideo.params.width+'&amp;vheight='+wthvideo.params.height+'&amp;vcolor='+wthvideo.params.color+'" />';
	markUp += '    <param name="wmode" value="transparent" />';
	markUp += '    <param name="allowscriptaccess" value="always" />';
	markUp += '    <param name="swfversion" value="9.0.45.0" />';
	markUp += '  </object>';
	markUp += '</div>';
	document.write(markUp);
}
function hideDiv() {
	wthvideo.hideDiv();
}
//wthvideo.drawVideo();

onlyOnce();

function thisMovie(movieName) {
         if (navigator.appName.indexOf("Microsoft") != -1) {
             return window[movieName];
         } else {
             return document[movieName];
         }
     }
	 