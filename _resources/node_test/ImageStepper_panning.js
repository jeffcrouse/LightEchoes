var EtherDream = require('./etherdream.js').EtherDream;
var fs = require('fs');
var PNGReader = require('pngjs');
var keyCommands = require('./keyCommands.js');
var colorMapper = require('./colorMapper.js');

var IMAGE_HEIGHT = 1000;
var imageName = 'bwtest.png';
var POINTSIZE = 38;//15 //38 full?  (offset needed probably)


var MAXINTENSITY = 65535
maxIntensity = MAXINTENSITY;

var EMPTYFRAME = [ { x: 0, y: 0, r: 0, g: 0, b: 0 } ];
var imageLoaded = false;
quitting = false;

var IP = '169.254.111.1';//'192.168.1.52';
var PORT = 7765;
var CIRCLE_POINTS = IMAGE_HEIGHT;
var numPixelsTall = CIRCLE_POINTS; 

var thePNG = new Object;
var laserX = -10000;

console.log('Connecting to '+IP+':'+PORT+' ...');

EtherDream.connect(IP, PORT, function(conn) {
	console.log('Connected');

	if (!conn) {
		console.log('Connected', conn);
		return;
	}

	function loadImage(imageName){
		console.log('reading image');
		fs.readFile('images/'+imageName, function(err, buffer){
    		var reader = new PNGReader(buffer);
   			 reader.parse(function(err, png){
       			 if (err) throw err;
        			thePNG = png;
        			console.log("imageLoaded");
        			imageLoaded = true;
        			// callback();
    			});
		});
	}

	function nextLine() {
		var framedata = [];
		if(imageLoaded){
			for(var i=0; i<CIRCLE_POINTS; i+=1) {
				var point = thePNG.getPixel(curLine,i);
				var pt = {};
				pt.x = laserX;
				pt.y = (i*POINTSIZE-(CIRCLE_POINTS/2*POINTSIZE))*-1;
				pt.r = 0;//colorMapper.laserRed(point[0]);//0;//maxIntensity;//colorsin(ip + phase);
				pt.g = 0;//colorMapper.laserGreen(point[1]);
				pt.b = colorMapper.laserBlue(point[2]);// maxIntensity;//colorsin(ip + (4.0 * Math.PI / 3.0) + phase);
				framedata.push(pt);
				// console.log(pt.y);
			}


			if(laserX<-10000){
				laserX=10000;
			}else{
				laserX-=100;
			}
			// console.log(framedata);
			
		} else {// image loaded
			framedata = EMPTYFRAME;
		}

			conn.write(framedata, 25000, function() { //30000
				console.log('frame written: '+curLine);
				curLine++;
				if(curLine>=1000) curLine = 0;
				if(!quitting){
					 nextLine();
					}else{
						console.log("goodbye!");
						process.exit();
					}
			});
	};// each line loop


	var curLine = 0;
	loadImage(imageName); 
	nextLine();
	
}); // connected to etherdream

  





