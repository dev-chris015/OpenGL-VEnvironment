#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform bool hasDiffuseTexture;
uniform vec4 materialColor;
uniform vec3 viewPos;

void main()
{
    // Directional key light (from upper-front-right)
    vec3 lightDir = normalize(vec3(1.0, 2.0, 1.5));
    vec3 norm     = normalize(Normal);

    // Ambient light - increased for more general brightness
    float ambient  = 0.45;
    
    // Diffuse light - multiplied by 1.2 to give it an extra boost
    float diffuse  = max(dot(norm, lightDir), 0.0) * 1.2;

    // Soft fill light - increased to soften shadows further
    vec3 fillDir   = normalize(vec3(-1.0, 0.5, -1.0));
    float fill     = max(dot(norm, fillDir), 0.0) * 0.35;

    // Specular light
    vec3 viewDir    = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    // Lower exponent (16 instead of 32) makes the highlight spread wider across the plastic
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    // Increased specular strength from 0.4 to 0.7 for more noticeable gloss
    vec3 specular   = vec3(0.7) * spec; 

    float lighting = ambient + diffuse + fill;

    vec4 texColor;
    if(hasDiffuseTexture) {
        texColor = texture(texture_diffuse1, TexCoords);
    } else {
        texColor = materialColor;
    }

    // Keep the true material color (black remains black)
    vec3 baseColorRgb = texColor.rgb;

    vec3 finalColor = (baseColorRgb * lighting) + specular;

    FragColor = vec4(finalColor, texColor.a);
}
