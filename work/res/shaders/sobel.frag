uniform sampler2D edge;
uniform sampler2D depthMap;
uniform sampler2D colorMap;
uniform float width;
varying float limit;

float intensity(in vec4 color, int i)
{
	return color[i];
}
float depthIntensity(in vec4 otherColor)
{
	return pow(otherColor.x, 400);
}

vec3 sobel(float step, vec2 center)
{
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
    float maxCol = mix(depth, norm, 0.05) * 2;
   // return vec3(maxCol,maxCol,maxCol);


	//return texture2D(colorMap, center).rgb;
	//return vec3(1.0,1.0,1.0);

    vec3 mixed=  mix(texture2D(colorMap, center).rgb,vec3(0,0,0), maxCol );
   return mixed;
 }

float rand(float n){return fract(sin(n) * 43758.5453123);}
void main(void)
{
	float step = 1.0/width;
	vec2 center = gl_TexCoord[0].st; //+ vec2(rand(gl_TexCoord[0].s) * .002, rand(gl_TexCoord[0].t) * .002);
	gl_FragColor.xyz = sobel(step,center);
}

