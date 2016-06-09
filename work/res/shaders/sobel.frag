uniform sampler2D edge;
uniform sampler2D depthMap;
uniform sampler2D colorMap;
uniform float width;
varying float limit;
uniform float time;
#define PI 3.14159265358979323846

float intensity(in vec4 color, int i)
{
	return color[i];
}
float depthIntensity(in vec4 otherColor)
{
	return pow(otherColor.x, 400);
}

float rand(vec2 c){
    return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p, float freq ){
    float unit = width/freq;
    vec2 ij = floor(p/unit);
    vec2 xy = mod(p,unit)/unit;
    //xy = 3.*xy*xy-2.*xy*xy*xy;
    xy = .5*(1.-cos(PI*xy));
    float a = rand((ij+vec2(0.,0.)));
    float b = rand((ij+vec2(1.,0.)));
    float c = rand((ij+vec2(0.,1.)));
    float d = rand((ij+vec2(1.,1.)));
    float x1 = mix(a, b, xy.x);
    float x2 = mix(c, d, xy.x);
    return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
    float persistance = .5;
    float n = 0.;
    float normK = 0.;
    float f = 4.;
    float amp = 1.;
    int iCount = 0;
    for (int i = 0; i<50; i++){
        n+=amp*noise(p, f);
        f*=2.;
        normK+=amp;
        amp*=persistance;
        if (iCount == res) break;
        iCount++;
    }
    float nf = n/normK;
    return nf*nf*nf*nf;
}

vec3 sobel(float step, vec2 center)
{
    vec2 oldCenter = center;
    float pNoiseVal =  pNoise(gl_TexCoord[0].st * 7000.0, 3);
    vec2 perlinNoise = vec2(pNoiseVal * 0.02, pNoiseVal * 0.02);
    center += perlinNoise;
    center *= 0.95;
	float norm = 0.0;
	for (int i =0; i < 3; i++){
    float tleft = intensity(texture2D(edge,center + vec2(-step,step)), i);
    float left = intensity(texture2D(edge,center + vec2(-step,0)), i);
    float bleft = intensity(texture2D(edge,center + vec2(-step,-step)), i);
    float top = intensity(texture2D(edge,center + vec2(0,step)), i);
    float bottom = intensity(texture2D(edge,center + vec2(0,-step)), i);
    float tright = intensity(texture2D(edge,center + vec2(step,step)), i);
    float right = intensity(texture2D(edge,center + vec2(step,0)), i);
    float bright = intensity(texture2D(edge,center + vec2(step,-step)), i);


	float x = tleft + 2.0*left + bleft - tright - 2.0*right - bright;
	float y = -tleft - 2.0*top - tright + bleft + 2.0 * bottom + bright;

    norm += sqrt((x*x) + (y*y));
    }
   // if (color > 0.95){return vec3(0.0,0.0,0.0);}

     float tleft = depthIntensity(texture2D(depthMap,center + vec2(-step,step)));
     float left = depthIntensity(texture2D(depthMap,center + vec2(-step,0)));
     float bleft = depthIntensity(texture2D(depthMap,center + vec2(-step,-step)));
     float top = depthIntensity(texture2D(depthMap,center + vec2(0,step)));
     float bottom = depthIntensity(texture2D(depthMap,center + vec2(0,-step)));
     float tright = depthIntensity(texture2D(depthMap,center + vec2(step,step)));
     float right = depthIntensity(texture2D(depthMap,center + vec2(step,0)));
     float bright = depthIntensity(texture2D(depthMap,center + vec2(step,-step)));

	 float x = tleft + 2.0*left + bleft - tright - 2.0*right - bright;
	 float y = -tleft - 2.0*top - tright + bleft + 2.0 * bottom + bright;
    float depth = sqrt((x*x) + (y*y));
    //if (color > 0.32){return vec3(0.0,0.0,0.0);}
    float maxCol = mix(depth, norm, 0.05) * 2.0;
   // return vec3(maxCol,maxCol,maxCol);

    center = oldCenter;
	//return texture2D(colorMap, center).rgb;
	//return vec3(1.0,1.0,1.0);
    pNoiseVal = pNoise(gl_TexCoord[0].st * 6000.0, 3);
    perlinNoise = vec2(pNoiseVal * 0.01, pNoiseVal * 0.01);
    center += perlinNoise;
    center *= 0.95;

    vec3 mixed=  mix(texture2D(colorMap, center).rgb,vec3(0,0,0), maxCol );
   return mixed;
 }

void main(void)
{
	float step = 1.0/width;
	vec2 center = gl_TexCoord[0].st; //+ vec2(rand(gl_TexCoord[0].s) * .002, rand(gl_TexCoord[0].t) * .002);
	gl_FragColor.xyz = sobel(step,center);
}

