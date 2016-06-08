uniform sampler2D rendered;
void main(void)
{
	vec2 center = gl_TexCoord[0].st; 
	gl_FragColor.xyz = texture2D(rendered, center).rgb;
}

