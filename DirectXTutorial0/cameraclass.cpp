////////////////////////////////////////////////////////////////////////////////
// ���ϸ�: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// ���� ������ ����Ű�� ���͸� �����Ѵ�.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR ������ �ε��Ѵ�.
	upVector = XMLoadFloat3(&up);

	// ī�޶��� ���� �� ��ġ�� �����Ѵ�.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// XMVECTOR ������ �ε��Ѵ�.
	positionVector = XMLoadFloat3(&position);

	// �⺻������ ī�޶� �ٶ󺸴� ������ �����Ѵ�.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// XMVECTOR ������ �ε��Ѵ�.
	lookAtVector = XMLoadFloat3(&lookAt);

	// ��(yaw, Y��), ��ġ(pitch, X��), ��(roll, Z��) ȸ������ �������� �����Ѵ�.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// yaw, pitch, roll ���� ����Ͽ� ȸ�� ����� �����Ѵ�.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// ȸ�� ����� ����� lookAt �� up ���͸� ��ȯ�Ͽ� ī�޶� �䰡 �ùٸ��� ȸ���ǵ��� �Ѵ�.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// ȸ���� ī�޶� ��ġ�� ��� ��ġ�� �̵���Ų��.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// ���������� ���ŵ� �� ���ͷκ��� �� ����� �����Ѵ�.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}
