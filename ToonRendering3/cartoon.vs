// vertex shader program

void main(void)
{
    // 位置座標を座標変換
	gl_Position = ftransform();
	
	// 法線と光源ベクトルとの内積(平行光源用)
	vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
	vec3 light = normalize(gl_LightSource[0].position.xyz);
	float lgtdot = dot(light, normal);

	// 法線と視線ベクトルとの内積(平行光源用)
    vec3 eye = - normalize(vec3(gl_ModelViewMatrix * gl_Vertex));
	float eyedot = dot(eye, normal);
    
	// テクスチャに割り当てる
    gl_TexCoord[0].s = lgtdot;
    gl_TexCoord[0].t = eyedot;
}
