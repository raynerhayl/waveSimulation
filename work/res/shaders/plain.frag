


#version 120

uniform sampler2D colorMap;
void main(void)
{
	vec2 center = gl_TexCoord[0].st; 
	center *= 0.98;
	gl_FragColor.rgb = texture2D(colorMap, center).rgb;
}

