#type vertex
#version 330 core
// 顶点属性
layout (location = 0) in vec3 aPos; // 坐标
layout (location = 1) in vec2 aTex; // 纹理坐标

// 输出到fragment shader的纹理坐标
out vec2 FragTex;

uniform mat4 _model;        // 模型变换矩阵
uniform mat4 _view;         // 观察矩阵
uniform mat4 _projection;   // 投影矩阵

void main() {
	// 纹理坐标
    FragTex = aTex;
	// 默认不进行变换
    gl_Position = vec4(aPos, 1.0); 
    // 实现最终矩阵变换:
	// TODO
    gl_Position=_projection*_view*_model*vec4(aPos,1.0);
}

#type fragment
#version 330 core

// 从vertex shader 输入的纹理坐标 (经过了重心插值)
in vec2 FragTex;

uniform sampler2D _outside;
uniform sampler2D _inside;

// 鼠标坐标
uniform vec2 _mousepos;
// xray范围
uniform float _range;

out vec4 FragOut;

void main() {
    FragOut = vec4(1.0);
    // 实现X-Ray (提示: 可以使用内建变量 gl_FragCoord 获取当前 fragment 所在的窗口坐标)
    vec2 dis = gl_FragCoord.xy;
    float di=sqrt((dis.x-_mousepos.x)*(dis.x-_mousepos.x)+(dis.y-_mousepos.y)*(dis.y-_mousepos.y));
    di=1/(1+exp((_range-di)*0.2));


	// TODO
   
    FragOut=mix(texture(_outside,FragTex),texture(_inside,FragTex),di);

}
