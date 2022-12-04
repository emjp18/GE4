#pragma once
#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include "pch.h"
#include "Array.h"
#include "Map.h"
#include <fbxsdk.h>
#include <mutex>
using namespace CUSTOM_CONTAINER;

#if _DEBUG
#pragma comment(lib, "..\\Libraries\\lib\\vs2019\\x64\\debug\\libfbxsdk.lib")
//#pragma comment(lib, "..\\Libraries\\lib\\vs2019\\x64\\debug\\libfbxsdk-md.lib")
//#pragma comment(lib, "..\\Libraries\\lib\\vs2019\\x64\\debug\\libxml2-mt.lib")
//#pragma comment(lib, "..\\Libraries\\lib\\vs2019\\x64\\debug\\zlib-md.lib")
//#pragma comment(lib, "..\\Libraries\\lib\\vs2019\\x64\\debug\\libfbxsdk-mt.lib")
//#pragma comment(lib, "..\\Libraries\\lib\\vs2019\\x64\\debug\\libxml2-md.lib")
//#pragma comment(lib, "..\\Libraries\\lib\\vs2019\\x64\\debug\\zlib-mt.lib")
//#pragma comment(lib, "..\\Libraries\\lib\\vs2019\\x64\\debug\\libfbxsdk.dll")
#else
#pragma comment(lib, "..\\Libraries\\lib\\vs2019\\x64\\release\\libfbxsdk.lib")
#endif
struct IndexStruct
{
	FbxDouble4 p;
	FbxDouble2 uv;
	FbxDouble4 n;
	FbxDouble4 t;
	FbxDouble4 b;
	bool operator==(const IndexStruct& other)
	{
		if (p.mData[0] == other.p.mData[0] &&
			p.mData[1] == other.p.mData[1] &&
			p.mData[2] == other.p.mData[2]
			&& uv.mData[0] == other.uv.mData[0]
			&& uv.mData[1] == other.uv.mData[1]
			&& n.mData[0] == other.n.mData[0]
			&& n.mData[1] == other.n.mData[1]
			&& n.mData[2] == other.n.mData[2]
			&& t.mData[0] == other.t.mData[0]
			&& t.mData[1] == other.t.mData[1]
			&& t.mData[2] == other.t.mData[2]
			&& b.mData[0] == other.b.mData[0]
			&& b.mData[1] == other.b.mData[1]
			&& b.mData[2] == other.b.mData[2])
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
struct ControlPoint
{

	int index;
	Array<float> skinWeight;
	Array<short> boneIndex;
	Array<FbxString> boneName;
	bool operator==(const ControlPoint& other)
	{
		if (index == other.index)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
class FileManager
{
    
public:
    FileManager();
	void Startup();
    // Prevents any type of copy or new instance
    FileManager(const FileManager&) = delete;
    void operator=(const FileManager&) = delete;
    
    
    static FileManager& Get()
    {
        static FileManager instance;
        return instance;
    }
	//Always left handed and always global space
	//Only one animation per file.
	FbxScene* GetScene();
	void GetRootNode();
	void Write();
	
private:
	FbxManager* m_sdkManager = nullptr;
	FbxScene* m_scene = nullptr;
	
	Array<GOFile::MESH> m_meshes;
	Array<Array<GOFile::VERTEX>> m_vertices;
	Array<Array<unsigned int>> m_indices;
	Array<GOFile::VERTEX> m_vertexArray;
	Array<unsigned int> m_indiceArray;
	Array<GOFile::MORPH_TARGET> m_blendshapes;
	Array<Array<GOFile::MORPH_VERTEX>> m_morphVertices;
	Array<Array<GOFile::MORPH_INDEX>> m_morphIndices;
	Array<Array<GOFile::MORPH_KEYFRAME>> m_morphKeyframes;
	Array<GOFile::MATERIAL> m_materials;
	Array<GOFile::LIGHT> m_lights;
	float m_fps = 30.0f;
	GOFile::FILE_INFO m_fileinfo;
	void GetNodes(FbxNode* node);
	void getMaterial();
	void getLight(FbxNode* lightNode);
	void GetVerticesAndIndices(Array<GOFile::VERTEX>& vertices, Array<unsigned int>& indices, FbxMesh* meshNode,ControlPoint* skinningData);
	ControlPoint* GetSkinningInformation(FbxSkin* skinNode, GOFile::MESH& mesh);
	void GetBlendShapes(FbxGeometry* geometry, fbxsdk::FbxScene* scene, FbxMesh* meshNode, GOFile::MESH& mesh);
	template <typename T>
	void SortIndices(Array<T>& arr, Array<unsigned int>& inarr, T value, int& c);
};
#endif

template<typename T>
inline void FileManager::SortIndices(Array<T>& arr, Array<unsigned int>& inarr, T value, int& c)
{
	auto it = arr.find(value);
	if (it == arr.end())
	{
		arr.push_back(value);
		inarr.push_back(c++);
	}
	else
	{
		inarr.push_back((unsigned int)arr.findIndex(value));
	}
}
