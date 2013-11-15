// fragment shader program

uniform sampler2D toontex;

void main (void)
{
	vec4 color = texture2D(toontex, gl_TexCoord[0].st);
	gl_FragColor = color;
}
