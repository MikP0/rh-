#pragma once

#include <string>
#include <cstring>
#include <cstdlib>
#include <map>
#include <vector>

struct aiMaterial;

enum TextureType
{
	TextureTypeDifffuse = 0,
	TextureTypeSpecularMap,
	TextureTypeAmbient,
	TextureTypeEmissive,
	TextureTypeHeightmap,
	TextureTypeNormalMap,
	TextureTypeSpecularPowerMap,
	TextureTypeDisplacementMap,
	TextureTypeLightMap,
	TextureTypeEnd
};

class ModelMaterial
{
	friend class ModelSK;

public:
	ModelMaterial(ModelSK& model);
	~ModelMaterial();

	ModelSK& GetModel();
	const std::string& Name() const;
	const std::map<TextureType, std::vector<std::wstring>*>& Textures() const;

private:
	static void InitializeTextureTypeMappings();
	static std::map<TextureType, unsigned int> sTextureTypeMappings;

	ModelMaterial(ModelSK& model, aiMaterial* material);
	ModelMaterial(const ModelMaterial& rhs);
	ModelMaterial& operator=(const ModelMaterial& rhs);

	ModelSK& mModel;
	std::string mName;
	std::map<TextureType, std::vector<std::wstring>*> mTextures;
};