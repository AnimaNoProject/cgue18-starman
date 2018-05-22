#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

class Camera
{
protected:

	float _pitch, _yaw, _fov;

	float _speed = 5.00f;
	const float _mouse_speed = 2.0f;

	vec3 _position;
	mat4 lookAt(vec3 const & eye, vec3 const & center, vec3 const & up);

public:
	mat4 _viewMatrix, _projMatrix;
	vec3 _eye, _center, _up;
	Camera(float fov, float aspect, float nearZ, float farZ);
	~Camera();
	mat4 getViewProjectionMatrix();
	virtual void update(float x, float y, bool up, bool down, bool left, bool right, float deltaTime);
	vec3 getPosition();
	void setSpeed(float speed);
	float getYaw();
	float getPitch();
};