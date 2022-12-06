#pragma once
#ifndef GAME_OBJECT_FILE_H
#define GAME_OBJECT_FILE_H
#include "Array.h"
#include <GO_Header.h>
using namespace CUSTOM_CONTAINER;
class Game_Object_File
{
public:
	Game_Object_File();
	void Clear();
	 Array<GOFile::MESH>& GetSetMeshes() { return m_meshes; }
	 Array<Array<GOFile::VERTEX>>& GetSetVertices() { return m_vertices; }
	 Array<Array<unsigned int>>& GetSetIndicies() { return m_indices; }
	 Array<GOFile::MORPH_TARGET>& GetSetBlendshapes() { return m_blendshapes; }
	 Array<Array<GOFile::MORPH_VERTEX>>& GetSetMorphVertices() { return m_morphVertices; }
	 Array<Array<GOFile::MORPH_INDEX>>& GetSetMorphIndicies() { return m_morphIndices; }
	 Array<Array<GOFile::MORPH_KEYFRAME>>& GetSetMorphKeyFrames() { return m_morphKeyframes; }
	 Array<GOFile::MATERIAL>& GetSetMaterials() { return m_materials; }
	 Array<GOFile::LIGHT>& GetSetLights() { return m_lights; }
	 Array<GOFile::SkeletonOffset>& GetSetSkeletonOffsetMatrices() { return m_offsetMatrices; }
	 Array<Array<GOFile::SkeletonKeyFrame>>& GetSetSkeletonKeyFrameMatrices() { return m_keyframeMatrices; }
	 GOFile::FILE_INFO& GetSetFileInfo() { return m_fileinfo; }
private:
	Array<GOFile::MESH> m_meshes;
	Array<Array<GOFile::VERTEX>> m_vertices;
	Array<Array<unsigned int>> m_indices;
	Array<GOFile::MORPH_TARGET> m_blendshapes;
	Array<Array<GOFile::MORPH_VERTEX>> m_morphVertices;
	Array<Array<GOFile::MORPH_INDEX>> m_morphIndices;
	Array<Array<GOFile::MORPH_KEYFRAME>> m_morphKeyframes;
	Array<GOFile::MATERIAL> m_materials;
	Array<GOFile::LIGHT> m_lights;
	Array<GOFile::SkeletonOffset> m_offsetMatrices;
	Array<Array<GOFile::SkeletonKeyFrame>> m_keyframeMatrices;
	GOFile::FILE_INFO m_fileinfo;


};

#endif //GAME_OBJECT_FILE_H