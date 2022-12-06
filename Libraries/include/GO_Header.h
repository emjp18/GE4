#pragma once
namespace GOFile
{
	enum class TYPE
	{
		MODEL,
		MATERIAL,
		SKELETON,
		BLEND_SHAPE,
		LIGHT,
		NONE
	};
	
	enum class LIGHT_TYPE
	{
		POINT,
		SPOT,
		DIR,
		NONE
	};
	
	//Only one skeleton per file, and one source for all the blend shapes per file.
	//Maximum materials per mesh is 10, maximum bones per Verttex is 4, Maximum name length is 100 the matrices is probably row major but not sure
	//worst case transpose it.
	struct FILE_INFO
	{
		unsigned short meshCount = 0;
		unsigned short materialCount = 0;
		unsigned short lightCount = 0;
		unsigned int skeletonKeyframeCount = 0;
		unsigned short skeletonBoneCount = 0;
		unsigned short BlendShapesCount = 0;
		float fps = 30.0f;
		float animationLenght = 0.0f;
	};
	struct MESH
	{
		char meshName[100] = { ' ' };
		unsigned int vertexCount = 0;
		unsigned int indexCount = 0;
		unsigned short bonePerVertexCount = 0;
		unsigned short blendShapeCount = 0;
		char materialName[10][100] = { {' '},{' '},{' '},{' '} ,{' '},{' '},{' '},{' '},{' '},{' '} };
	};

	struct VERTEX
	{
		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;
		float u = 0.0f;
		float v = 0.0f;
		float normalX = 0.0f;
		float normalY = 0.0f;
		float normalZ = 0.0f;
		float tangentX = 0.0f;
		float tangentY = 0.0f;
		float tangentZ = 0.0f;
		float biNormalX = 0.0f;
		float biNormalY = 0.0f;
		float biNormalZ = 0.0f;
		float skinWeight[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		short boneIndex[4] = { -1,-1,-1,-1 };
		char boneName[4][100] = { {' '},{' '},{' '},{' '} };
	};

	struct MORPH_TARGET
	{
		char name[100] = { ' ' };
		char sourceName[100] = { ' ' };
		unsigned int vertexCount = 0;
		unsigned int indexCount = 0;
		unsigned int morphKeyframeCount = 0;
	};

	struct MORPH_VERTEX
	{
		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;
		float normalX = 0.0f;
		float normalY = 0.0f;
		float normalZ = 0.0f;
		float tangentX = 0.0f;
		float tangentY = 0.0f;
		float tangentZ = 0.0f;
		float biNormalX = 0.0f;
		float biNormalY = 0.0f;
		float biNormalZ = 0.0f;
	};

	struct MORPH_KEYFRAME
	{
		float weight = 0.0f;
		float timeStamp = 0.0f;
	};

	struct MORPH_INDEX
	{

		unsigned int sourceIndex;
		unsigned int targetIndex;
	};

	struct SkeletonOffset
	{
		unsigned short boneIndex;
		char boneName[100] = { ' ' };
		char boneParentName[100] = { ' ' };
		//not recursive
		unsigned short boneChildCount = 0;
		//
		float m[4][4] = { {0.0f,0.0f,0.0f,0.0f}
			, {0.0f,0.0f,0.0f,0.0f}
			, {0.0f,0.0f,0.0f,0.0f}
			, {0.0f,0.0f,0.0f,0.0f} };

	};
	struct SkeletonKeyFrame
	{
		unsigned short boneIndex;
		float s[4] = { 0.0f,0.0f,0.0f,0.0f };
		//rotationquaternion
		float r[4] = { 0.0f,0.0f,0.0f,0.0f };
		float t[4] = { 0.0f,0.0f,0.0f,0.0f };
	};

	struct MATERIAL
	{
		char name[100] = { ' ' };
		float ka[3];
		float kd[3];
		//w is shininess
		float ks[4];
	};

	struct LIGHT
	{
		LIGHT_TYPE type = LIGHT_TYPE::NONE;
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;
		float dirX = 0.0f;
		float dirY = 0.0f;
		float dirZ = 0.0f;
		//in radians
		float fov = 0.0f;
		float intensity = 0.0f;
	};
}