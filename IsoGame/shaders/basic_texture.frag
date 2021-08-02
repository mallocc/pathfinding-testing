#version 400 core



// ins
in vec3 o_color;
in vec2 o_uv;

// uniforms
uniform mat4 u_m;
uniform mat4 u_v;
uniform mat4 u_p;

uniform sampler2D u_tex;

out vec4 out_color;

void main() 
{	
	vec4 final_color = vec4(o_color,0) + texture2D(u_tex,o_uv);

// apply fragment color
	out_color = final_color;
}
