var EtherDream = require('./etherdream.js').EtherDream;
var fs = require('fs');
var PNGReader = require('pngjs');
var keyCommands = require('./keyCommands.js');
var colorMapper = require('./colorMapper.js');

var IMAGE_HEIGHT = 1000;
var imageName = 'text_01.png';
var POINTSIZE = 63;//15 //38 full?  (offset needed probably)
var drawSpeed = 1;// (draw every line? = 1)
var looping = false;


var MAXINTENSITY = 65535
maxIntensity = MAXINTENSITY;

var EMPTYFRAME = [ { x: 0, y: 0, r: 0, g: 0, b: 0 } ];
var imageLoaded = false;
var imageWidth = 1000;
quitting = false;

var IP = '169.254.110.253';//'192.168.1.52';
var PORT = 7765;
var CIRCLE_POINTS = IMAGE_HEIGHT;
var numPixelsTall = CIRCLE_POINTS;

var repeatCount = 0;
var NUM_REPEATS = 0;

var thePNG = new Object;


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
        			        			console.log("imageWidth: "+ png.getWidth());

        			imageWidth = png.getWidth();
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
				pt.y = 2000;
				pt.x = (i*POINTSIZE-(CIRCLE_POINTS/2*POINTSIZE))*-1-3000;
				pt.r = colorMapper.laserRed(point[0]);//0;//maxIntensity;//colorsin(ip + phase);
				pt.g = colorMapper.laserGreen(point[1]);
				pt.b = colorMapper.laserBlue(point[2]);// maxIntensity;//colorsin(ip + (4.0 * Math.PI / 3.0) + phase);
				framedata.push(pt);
				// console.log(pt.y);
			}
				

			for(var i=CIRCLE_POINTS-1;i>=0; i-=Math.abs(CIRCLE_POINTS-1/4)) { //50 5 100
				var point = thePNG.getPixel(curLine,i);
				var pt = {};
				pt.x = 0;
				pt.y = ((i)*POINTSIZE-(CIRCLE_POINTS/2*POINTSIZE))*-1;
				pt.r = (point[0]/255)*maxIntensity;//0;//maxIntensity;//colorsin(ip + phase);
				pt.g = (point[1]/255)*maxIntensity;
				pt.b = (point[2]/255)*maxIntensity;;// maxIntensity;//colorsin(ip + (4.0 * Math.PI / 3.0) + phase);
				framedata.push(pt);
								// console.log(pt.y);

			}

			// console.log(framedata);
			
		} else {// image loaded
			framedata = EMPTYFRAME;
		}

			conn.write(framedata, 30000, function() { //30000
				console.log('frame written: '+curLine);
				if(repeatCount < NUM_REPEATS){
					repeatCount++;
				} else {
					curLine+=drawSpeed;
				}
				if(curLine>=imageWidth){
					if(looping == false){
						console.log("goodbye!");
						process.exit();
					}else{
						curLine = 0;
					}
				} 
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

  





