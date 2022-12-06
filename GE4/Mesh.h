#pragma once
#ifndef MESH_H
#define MESH_H
#include "GameObject.h"
class Camera {}; //forward declaration
struct INSTANCE_DATA
{
    XMFLOAT4X4 world;
    XMFLOAT4X4 inverseWorld;
};
class Mesh :
    public GameObject
{
public:
    Mesh(Vector3& scale, Quaternion& rotation, Vector3& position,
        bool readwriteBuffer);
    bool Draw(Camera* camera, ID3D11DeviceContext* context);
    // Inherited via GameObject
    virtual void SetRotation(Quaternion& r, int id = 0) override;
    virtual void SetTranslation(Vector3& t, int id = 0) override;
    void setIndices(ID3D11Device* device,  Array<unsigned int>& indices);
    void SetInstanceData(ID3D11Device* device,  Array<INSTANCE_DATA>& instanceData);
    void SetVertices(ID3D11Device* device, Array<VERTEX>& vertices);
    bool GetInstanced() { return m_instanced; }
    virtual void SetScale(Vector3& t, int id = 0) override;
    bool g_cull = true;
    bool g_render = true;
    bool g_update = true;
    bool g_renderClockWise = false;
    string g_name;
private:
    WRL::ComPtr<ID3D11Buffer> m_vertexAndInstanceBfr[2] = { nullptr, nullptr };
    WRL::ComPtr<ID3D11Buffer> m_indexBfr = nullptr;
    bool m_instanced = true;
    D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    Array<VERTEX> m_vertices;
    Array<unsigned int> m_indices;
    Array<INSTANCE_DATA> m_instanceData;
    bool m_readwritebuffer;
    void CreateVertexBuffer(ID3D11Device* device, void* data, UINT size);
    void CreateIndexBuffer(ID3D11Device* device, void* data, UINT size);
    void CreateInstanceBuffer(ID3D11Device* device, void* data, UINT size);
};

#endif //MESH_H