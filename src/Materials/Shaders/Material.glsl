struct Material {
    vec4 albedo;
    vec4 ambientColor;
    vec4 diffuseColor;
    vec4 specularColor;

    bool hasBaseColorTexture;
    bool hasAmbientColorTexture;
    bool hasDiffuseColorTexture;
    bool hasSpecularColorTexture;
    bool hasNormalMap;
    bool hasMetallicRoughnessTexture;

    uint baseColorTextureIndex;
    uint ambientColorTextureIndex;
    uint diffuseColorTextureIndex;
    uint specularColorTextureIndex;
    uint normalMapIndex;
    uint metallicRoughnessTextureIndex;
};