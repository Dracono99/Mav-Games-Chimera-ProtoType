#pragma once
#include "d3dUtil.h"

class FreeForm3DCamera
{
private:
	D3DXVECTOR3 m_position; // camera position
	float m_yaw;  // rotation around the y axis
	float m_pitch; // rotation around the x axis
	float m_roll; // rotation around the z axis
	D3DXVECTOR3 m_up,m_look,m_right; // camera axis

	float RestrictAngleTo360Range(float angle) const;
public:
	FreeForm3DCamera(D3DXVECTOR3 startPos);
	~FreeForm3DCamera(void);

	void CalculateViewMatrix(D3DXMATRIX *viewMatrix);

	// Gets
	float GetYaw() const {return m_yaw;}
	float GetPitch() const {return m_pitch;}
	float GetRoll() const {return m_roll;}
	D3DXVECTOR3 GetPosition() const {return m_position;}
	D3DXVECTOR3 GetLook() const {return m_look;}

	// Move operations
	void MoveForward(float amount) {m_position+=m_look*amount;}
	void MoveRight(float amount) {m_position+=m_right*amount;}
	void MoveUp(float amount) {m_position+=m_up*amount;}

	// Rotations
	void Yaw(float amount); // rotate around y axis
	void Pitch(float amount); // rotate around x axis
	void Roll(float amount); // rotate around z axis
	void CalcViewMatrixFromVec3PosAndQuaternion(D3DXVECTOR3& pos,D3DXQUATERNION& quat,D3DXMATRIX& mat);
};
