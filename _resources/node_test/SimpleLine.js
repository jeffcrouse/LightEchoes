var EtherDream = require('./etherdream.js').EtherDream;
var keyCommands = require('./keyCommands.js');


quitting = false;

var maxIntensity = 65535;

// function sendframe(connection, data, callback) {
// 	//console.log('send frame');
// 	connection.write(data, 60000, function() {
// 	//	console.log('frame written.');
// 		callback();
// 	});
// }

var IP = '169.254.159.138';//'192.168.1.52';
var PORT = 7765;
var CIRCLE_POINTS = 700;

console.log('Connecting to '+IP+':'+PORT+' ...');

EtherDream.connect(IP, PORT, function(conn) {

	console.log('Connected', conn);
	if (!conn) {
		return;
	}

	function nextframe(phase) {
		var framedata = [];
		for(var i=0; i<CIRCLE_POINTS; i++) {
			var pt = {};
			var ip = i * 2.0 * Math.PI / CIRCLE_POINTS;
			pt.x = 0;
			pt.y = Math.cos(phase * 0.3 + ip * 3) * 20000;
			pt.r = maxIntensity;//colorsin(ip + phase);
			pt.g = maxIntensity;//colorsin(ip + (2.0 * Math.PI / 3.0) + phase);
			pt.b = maxIntensity;//colorsin(ip + (4.0 * Math.PI / 3.0) + phase);
						// console.log(pt);

			framedata.push(pt);
		}

		console.log('send frame, phase='+phase);
		conn.write(framedata, 60000, function() {
			// console.log('frame written.');
			if(!quitting){
					 nextframe(phase + 1.0 / 50.0);
					}else{
						console.log("goodbye!");
						process.exit();
					}
		});
	};

	nextframe(0);

});







