#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D spriteTexture;
//uniform vec3 lightColor;
//
//float near = 0.5;
//float far  = 1000.0;
//
//float LinearizeDepth(float depth)
//{
//    float z = depth * 2.0 - 1.0; // back to NDC
//    return (2.0 * near * far) / (far + near - z * (far - near));
//}

void main()
{
    vec4 textureColor = texture(spriteTexture, TexCoord);
    if (textureColor.a <= 0)
        discard;
    //vec3 lightColor = vec3(0.6f, 0.7f, 0.0f);
    //FragColor = vec4(textureColor.xyz * lightColor, textureColor.w);
    FragColor = textureColor;
//    float depth = LinearizeDepth(gl_FragCoord.z) / far;
//    FragColor = vec4(vec3(depth), 1.0);
}
