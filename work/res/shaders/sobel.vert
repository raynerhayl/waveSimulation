// Contours vertex shader
// Jeroen Baert
// www.forceflow.be

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;
	gl_TexCoord[2] = gl_MultiTexCoord2;
	// position transformation
	gl_Position = ftransform();
} 
