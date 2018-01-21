#version 330 core


#ifdef VERTEX
#definesub INOUT out
#endif
#ifdef FRAGMENT
#definesub INOUT in
#endif

// output of the geometry shader, input to the fragment shader
#sub INOUT  VertexData {
	vec2 uv;
	vec3 normal;
} vertex_#sub INOUT ;



//---------------------------------------------------------------------------------------------

#ifdef VERTEX

in vec3 a_vertex_position_modelspace;
in vec2 a_vertex_UV;
in vec3 a_vertex_normal_modelspace;

uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;

void main()
{
	gl_Position = u_projection_matrix * u_view_matrix * vec4(a_vertex_position_modelspace, 1.0); //vec4(a_vertex_position_modelspace.x ,a_vertex_position_modelspace.y, 0.0, 1.0);
	vertex_out.uv = a_vertex_UV;
	vertex_out.normal = a_vertex_normal_modelspace;
}


#endif





//---------------------------------------------------------------------------------------------

#ifdef FRAGMENT

uniform sampler2D u_texture_0;

layout(location = 0) out vec4 color;

void main()
{
	vec4 tex0 = texture(u_texture_0, vertex_in.uv);
	//color = vec4(vertex_in.uv.x, vertex_in.uv.y, 0.0, 1.0) * tex0;
	color = vec4(vertex_in.normal*0.5+0.5, 1.0); 
}


#endif