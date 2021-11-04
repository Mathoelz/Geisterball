#version 400

uniform vec3 EyePos;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;

uniform sampler2D MixTex; // for exercise 3
uniform sampler2D DetailTex[2]; // for exercise 3
uniform float k;
uniform vec3 Scaling;

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;
out vec4 FragColor;

float sat( in float a)
{
    return clamp(a, 0.0, 1.0);
}

// https://learnopengl.com/Advanced-OpenGL/Blending
// http://www.cs.toronto.edu/~jacobson/phong-demo/

void main()
{
    vec3 N      = normalize(Normal);
    vec3 L      = normalize(LightPos); // light is treated as directional source
    vec3 D      = EyePos-Position;
    float Dist  = length(D);
    vec3 E      = D/Dist;
    vec3 R      = reflect(-L,N);
    
    vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
    vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(R,E)), SpecularExp);
    
    // Exercise 3
    
    // TODO: Add texture blending code here..
    
    // Texel_final = (Texel_Mixtextur * Texel_Steintextur + (1-Texel_Mixtextur)*Texel_Rasentextur)*Reflexionsergebnisfarbe.
    
    // ----- //
    
        // Reflexionsergebnisfarbe (I_out = I_ambient + I_diffus + I_speular)
        // https://de.wikipedia.org/wiki/Phong-Beleuchtungsmodell
    vec4 ReflectionColor = vec4((AmbientColor + DiffuseComponent + SpecularComponent),1);
    
        // Texel_Mixtextur
    vec4 MixTextur   = texture(MixTex, Texcoord);
        // Texel_Rasentextur
    vec4 grassTextur = texture(DetailTex[0], Texcoord);
        // Texel_Steintextur
    vec4 stoneTextur = texture(DetailTex[1], Texcoord);
    
    // TODO: Interpolation
    /*vec4 linearInterpolation = vec4(0);
    if(Scaling.y > 0){
        float weigth = (MixTextur.r + MixTextur.g + MixTextur.b ) / 3.0f;
        linearInterpolation = mix(grassTextur, stoneTextur, weigth);
    }
    
    FragColor = linearInterpolation  * ReflectionColor;*/
    
    FragColor = (MixTextur * stoneTextur + (1-MixTextur) * grassTextur) * ReflectionColor;
}
