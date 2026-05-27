#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform bool hasDiffuseTexture;
uniform vec4 materialColor;
uniform vec3 viewPos;
uniform samplerCube skybox;

//aplico reflejos y refraccion

vec3 applyReflectionAndRefraction(vec3 normal, vec3 viewDir, float ratio) {
    vec3 I = -viewDir; // Incident vector from camera to fragment
    // Reflection
    vec3 reflectDir = reflect(I, normal);
    vec3 reflectionColor = texture(skybox, reflectDir).rgb;
    
    // Refraction (e.g. air to glass)
    vec3 refractDir = refract(I, normal, ratio);
    vec3 refractionColor = texture(skybox, refractDir).rgb;
    
    // Simple mix of reflection and refraction
    return mix(refractionColor, reflectionColor, 0.5);
}

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

    vec3 envColor = applyReflectionAndRefraction(norm, viewDir, 1.0 / 1.52);

    vec3 finalColor = (baseColorRgb * lighting) + specular;
    
    // Mix the object's lit color with the environment reflection/refraction
    finalColor = mix(finalColor, envColor, 0.4); // 40% environment

    FragColor = vec4(finalColor, texColor.a);
}
