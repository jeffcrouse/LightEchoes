


var MIN_r = 41; //20?
var MIN_g = 41;
var MIN_b = 30;//80

var MAX_r = 255;
var MAX_g = 255;
var MAX_b = 255;

function map(value, low1, high1, low2, high2) {
    return low2 + (high2 - low2) * (value - low1) / (high1 - low1);
}

module.exports = {
 laserRed: function(value){
    return (map(value,0,255,MIN_r,MAX_r)/255)*maxIntensity;
},
 laserGreen: function(value){
    return (map(value,0,255,MIN_g,MAX_g)/255)*maxIntensity;
},
 laserBlue: function(value){
    return (map(value,0,255,MIN_b,MAX_b)/255)*maxIntensity;
}
};