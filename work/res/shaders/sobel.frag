uniform sampler2D edge;
uniform sampler2D depthMap;
uniform sampler2D colorMap;
uniform float width;
varying float limit;

float intensity(in vec4 color)
{
	//return sqrt((color.x*color.x)+(color.y*color.y)+(color.z*color.z));
	return (color.x + color.y + color.z)/3.0;
}

float depthIntensity(in vec4 color)
{
	return pow(color.x, 300);
}

vec3 sobel(float step, vec2 center)
{
    float tleft = intensity(texture2D(edge,center + vec2(-step,step)));
    float left = intensity(texture2D(edge,center + vec2(-step,0)));
    float bleft = intensity(texture2D(edge,center + vec2(-step,-step)));
    float top = intensity(texture2D(edge,center + vec2(0,step)));
    float bottom = intensity(texture2D(edge,center + vec2(0,-step)));
    float tright = intensity(texture2D(edge,center + vec2(step,step)));
    float right = intensity(texture2D(edge,center + vec2(step,0)));
    float bright = intensity(texture2D(edge,center + vec2(step,-step)));


	float x = tleft + 2.0*left + bleft - tright - 2.0*right - bright;
	float y = -tleft - 2.0*top - tright + bleft + 2.0 * bottom + bright;
    float color = sqrt((x*x) + (y*y));
    if (color > 0.25){return vec3(0.0,0.0,0.0);}

     tleft = depthIntensity(texture2D(depthMap,center + vec2(-step,step)));
     left = depthIntensity(texture2D(depthMap,center + vec2(-step,0)));
     bleft = depthIntensity(texture2D(depthMap,center + vec2(-step,-step)));
     top = depthIntensity(texture2D(depthMap,center + vec2(0,step)));
     bottom = depthIntensity(texture2D(depthMap,center + vec2(0,-step)));
     tright = depthIntensity(texture2D(depthMap,center + vec2(step,step)));
     right = depthIntensity(texture2D(depthMap,center + vec2(step,0)));
     bright = depthIntensity(texture2D(depthMap,center + vec2(step,-step)));

	 x = tleft + 2.0*left + bleft - tright - 2.0*right - bright;
	 y = -tleft - 2.0*top - tright + bleft + 2.0 * bottom + bright;
     color = sqrt((x*x) + (y*y));
    if (color > 0.05){return vec3(0.0,0.0,0.0);}


	return texture2D(colorMap, center).rgb;
	//return vec3(1.0,1.0,1.0);
 }

float rand(float n){return fract(sin(n) * 43758.5453123);}
void main(void)
{
	float step = 1.0/width;
	vec2 center = gl_TexCoord[0].st; //+ vec2(rand(gl_TexCoord[0].s) * .002, rand(gl_TexCoord[0].t) * .002);
	gl_FragColor.xyz = sobel(step,center);
}

