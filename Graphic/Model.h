#pragma once
// Std. Includes
#include <string>
#include <map>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"


using namespace std;


class Model
{
public:
	Model(string path)
	{
		Assimp::Importer importer;
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		// Check for errors
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// Retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// Process ASSIMP's root node recursively
		traverseNode(scene->mRootNode);
	}

	void Draw(Shader *shader)
	{
		for each (Mesh m in meshes)
			m.Draw(shader);
	}

private:
	const aiScene* scene;
	string directory;
	vector<Mesh> meshes;
	map<string, Texture> allTextures;

	void traverseNode(aiNode* node)
	{
		// Process each mesh located at the current node
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh));
		}
		
		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			traverseNode(node->mChildren[i]);
		}
	}

	Mesh processMesh(aiMesh* mesh)
	{
		vector<Texture> textures;

		// 每个mesh只有一个material，可以多个漫射贴图，多个高光贴图……
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// 假设着色器中贴图命名规则如下，N 取 1，2，3……
			// Diffuse: texture_diffuseN
			// Specular: texture_specularN
			// Normal: texture_normalN

			// 1. Diffuse maps
			vector<Texture> diffuseMaps = loadTypeTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. Specular maps
			vector<Texture> specularMaps = loadTypeTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(mesh, textures);
	}

	vector<Texture> loadTypeTextures(aiMaterial* mat, aiTextureType type, string typeStr)
	{
		vector<Texture> typeTextures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString aiStr;
			mat->GetTexture(type, i, &aiStr);
			string str = directory + "/" + aiStr.C_Str();
			if (allTextures.find(str)!=allTextures.end())
			{
				continue;
			}
			cout << str << endl;
			Texture texture(str, typeStr);
			typeTextures.push_back(texture);
			allTextures.insert(make_pair(str, texture));
		}
		return typeTextures;
	}
};
