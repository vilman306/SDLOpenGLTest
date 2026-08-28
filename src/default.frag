#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D spriteTexture;
//uniform vec3 lightColor;

void main()
{
    vec4 textureColor = texture(spriteTexture, TexCoord);
    if (textureColor.a <= 0)
        discard;
//    FragColor = vec4(textureColor.xyz * lightColor, textureColor.a);
    FragColor = textureColor;
}
