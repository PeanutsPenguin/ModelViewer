#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec3 TexCoord;

uniform sampler3D textures[];
uniform int nbtextures;

vec4 GenerateTextures() 
{
    if (!(nbtextures > 0)) return vec4(0, 1, 1, 1); 

    vec4 Texture = texture(textures[0], TexCoord);

    if (nbtextures > 1) Texture = mix(Texture, texture(textures[1], TexCoord), 0.5);

    if (nbtextures > 2) Texture = mix(Texture, texture(textures[2], TexCoord), 0.5);

    if (nbtextures > 3) Texture = mix(Texture, texture(textures[3], TexCoord), 0.2);

    return Texture;
}

void main()
{  
    vec4 Texture = GenerateTextures();
    FragColor = Texture;
}