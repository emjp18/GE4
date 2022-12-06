#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(const Vector3& scale, const Quaternion& rotation, const Vector3& position,
	bool readwriteBuffer)
	:GameObject( scale, rotation, position )
{
	m_readwritebuffer = readwriteBuffer;
}

Mesh::~Mesh()
{
}

bool Mesh::Draw(Camera* camera, ID3D11DeviceContext* context)
{
    return false;
}

void Mesh::SetRotation(const Quaternion& r, int id)
{
	m_transform->r[id] = r;

}

void Mesh::SetTranslation(const Vector3& t, int id)
{
	m_transform->t[id] = t;
}

void Mesh::setIndices(ID3D11Device* device, Array<unsigned int>& indices)
{
	m_indices = indices;
	CreateIndexBuffer(device, m_indices.Data(), sizeof(unsigned int) * (UINT)indices.size());
	
	
}

void Mesh::CreateVertexBuffer(ID3D11Device* device, void* data, UINT size)
{
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DEFAULT;
	if (m_readwritebuffer)
	{
		vbd.Usage = D3D11_USAGE_DYNAMIC;
	}
	vbd.ByteWidth = size;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	vbd.MiscFlags = 0;
	vbd.CPUAccessFlags = 0;
	if (m_readwritebuffer)
	{
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	D3D11_SUBRESOURCE_DATA vData = {};
	vData.pSysMem = data;
	

	HRESULT hr = device->CreateBuffer(&vbd, &vData, m_vertexAndInstanceBfr[0].GetAddressOf());
	assert(SUCCEEDED(hr));
}

void Mesh::CreateIndexBuffer(ID3D11Device* device, void* data, UINT size)
{
	D3D11_BUFFER_DESC indexBDesc = {};


	indexBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBDesc.ByteWidth = size;
	indexBDesc.CPUAccessFlags = 0;
	indexBDesc.MiscFlags = 0;
	indexBDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA indexdata = {};
	indexdata.pSysMem = data;

	HRESULT hr = device->CreateBuffer(&indexBDesc, &indexdata, m_indexBfr.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void Mesh::SetInstanceData(ID3D11Device* device,  Array<INSTANCE_DATA>& instanceData)
{

	m_instanceData = instanceData;
	for (int i = 0; i < m_instanceData.size(); i++)
	{
		Matrix world = m_instanceData[i].world;
		Vector3 s;
		Vector3 t;
		Quaternion r;
		world.Decompose(s, r, t);
		m_transform->s.push_back(s);
		m_transform->r.push_back(r);
		m_transform->t.push_back(t);
	}
	CreateInstanceBuffer(device, m_instanceData.Data(), sizeof(INSTANCE_DATA) * (UINT)instanceData.size());
	
}

void Mesh::SetVertices(ID3D11Device* device, Array<VERTEX>& vertices)
{
	m_vertices = m_vertices;
	CreateVertexBuffer(device, m_vertices.Data(), sizeof(VERTEX) * (UINT)m_vertices.size());
}

void Mesh::SetScale(const Vector3& t, int id)
{
	m_transform->t[id] = t;
}

void Mesh::CreateInstanceBuffer(ID3D11Device* device, void* data, UINT size)
{
	D3D11_BUFFER_DESC instBuffDesc = {};

	instBuffDesc.StructureByteStride = 0;
	instBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	instBuffDesc.ByteWidth = size;
	instBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instBuffDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA instData = {};
	instData.SysMemSlicePitch = 0;
	instData.SysMemPitch = 0;
	instData.pSysMem = data;
	HRESULT hr = device->CreateBuffer(&instBuffDesc, &instData, m_vertexAndInstanceBfr[1].GetAddressOf());

	assert(SUCCEEDED(hr));
}
