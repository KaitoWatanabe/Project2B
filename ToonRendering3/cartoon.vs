// vertex shader program

void main(void)
{
    // �ʒu���W�����W�ϊ�
	gl_Position = ftransform();
	
	// �@���ƌ����x�N�g���Ƃ̓���(���s�����p)
	vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
	vec3 light = normalize(gl_LightSource[0].position.xyz);
	float lgtdot = dot(light, normal);

	// �@���Ǝ����x�N�g���Ƃ̓���(���s�����p)
    vec3 eye = - normalize(vec3(gl_ModelViewMatrix * gl_Vertex));
	float eyedot = dot(eye, normal);
    
	// �e�N�X�`���Ɋ��蓖�Ă�
    gl_TexCoord[0].s = lgtdot;
    gl_TexCoord[0].t = eyedot;
}
