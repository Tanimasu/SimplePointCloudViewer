// 阶段二：片段着色器
// 输入：顶点着色器传递的颜色
// 输出：最终像素颜色

#version 330 core

in vec3 vColor;
out vec4 FragColor;

void main()
{
    // 简单的颜色输出
    FragColor = vec4(vColor, 1.0);

    // 可选：让点变成圆形（而非方形）
    // vec2 coord = gl_PointCoord - vec2(0.5);
    // if(length(coord) > 0.5) discard;
}
