#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    
    // Discard transparent pixels so they don't block things behind them
    if (texelColor.a < 0.5) discard;
    
    finalColor = texelColor * colDiffuse * fragColor;
}
