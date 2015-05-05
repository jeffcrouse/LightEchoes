varying vec2 texcoord;
uniform sampler2DRect image;


uniform float brightness;
uniform float redMin;
uniform float redMax;
uniform float greenMin;
uniform float greenMax;
uniform float blueMin;
uniform float blueMax;

float map(float value, float low1, float high1, float low2, float high2) {
    return low2 + (high2 - low2) * (value - low1) / (high1 - low1);
}

void main (void)
{
    vec3 inColor = texture2DRect(image, texcoord).rgb;
    vec3 outColor;
    
    outColor.r = map(inColor.r, 0.0, 1.0, redMin, redMax);
    outColor.g = map(inColor.g, 0.0, 1.0, greenMin, greenMax);
    outColor.b = map(inColor.b, 0.0, 1.0, blueMin, blueMax);
    
    outColor *= brightness;
    
    gl_FragColor = vec4 (outColor, 1.0);
}