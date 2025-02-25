   #version 460

   in vec3 FragPos;
   in vec3 Normal;
   in vec3 LightDir;
   in vec2 TexCoord; // Receive texture coordinates from the vertex shader

   out vec4 FragColor;

   struct LightInfo {
       vec4 position;
       vec3 La; // Ambient light
       vec3 Ld; // Diffuse light
       vec3 Ls; // Specular light
       vec3 direction; // Spotlight direction
       float cutoff; // Spotlight cutoff angle (in degrees)
   };

   struct MaterialInfo {
       vec3 Ka; // Ambient reflectivity
       vec3 Kd; // Diffuse reflectivity
       vec3 Ks; // Specular reflectivity
       float Shininess; // Specular shininess factor
   };

   uniform LightInfo Light;
   uniform MaterialInfo Material;
   uniform sampler2D Texture; // Base color texture
   uniform sampler2D MetallicTexture; // Metallic texture
   uniform sampler2D NormalTexture; // Normal map texture
   uniform sampler2D HeightTexture; // Height texture
   uniform sampler2D RoughnessTexture; // Roughness texture

   // Fog parameters
   uniform vec3 fogColor;
   uniform float fogDensity;

   void main()
   {
       // Calculate ambient, diffuse, and specular components
       vec3 ambient = Light.La * Material.Ka;

       // Sample the normal map
       vec3 normalMap = texture(NormalTexture, TexCoord).rgb;
       vec3 perturbedNormal = normalize(normalMap * 2.0 - 1.0); // Convert from [0, 1] to [-1, 1]

       vec3 n = normalize(perturbedNormal);
       vec3 s = normalize(LightDir);
       float sDotN = max(dot(s, n), 0.0);
       vec3 diffuseLight = Light.Ld * Material.Kd * sDotN;

       vec3 v = normalize(-FragPos);
       vec3 r = reflect(-s, n);
       float rDotV = max(dot(r, v), 0.0);
       vec3 specularLight = Light.Ls * Material.Ks * pow(rDotV, Material.Shininess);

       vec3 color = ambient + diffuseLight + specularLight;

       // Apply base color texture
       vec3 baseColor = texture(Texture, TexCoord).rgb;
       float metallic = texture(MetallicTexture, TexCoord).r;
       float roughness = texture(RoughnessTexture, TexCoord).r;

       // Ensure metallic is not fully 1.0 everywhere
       metallic = clamp(metallic, 0.0, 0.9); // Reduce maximum metallic effect

       // Mix metallic properties properly
       vec3 diffuse = baseColor * (1.0 - metallic); // Keep some diffuse lighting
       vec3 specular = mix(vec3(0.04), baseColor, metallic); // Avoid complete blackness

       // Apply height texture for bump mapping
       float height = texture(HeightTexture, TexCoord).r;
       vec3 perturbedNormalHeight = normalize(n + height * vec3(0.0, 0.0, 1.0)); // Simple bump mapping

       // Blend diffuse and specular components
       vec3 finalColor = diffuse + specular * (1.0 - roughness);

       // Fog calculations
       float distance = length(FragPos);
       float fogFactor = exp(-fogDensity * distance);
       fogFactor = clamp(fogFactor, 0.0, 1.0);

       finalColor = mix(fogColor, finalColor, fogFactor);

       // Output final color
       FragColor = vec4(finalColor, 1.0);
   }
   