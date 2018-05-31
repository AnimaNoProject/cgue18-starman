#include "RPlayer.h"

RPlayer::RPlayer()
{
}

RPlayer::RPlayer(Model* model, Camera* camera, _Shader* shader) : _speed(0), _real_speed(0), _health(100)
{
	_model = model;
	_model->transform(translate(mat4(1), vec3(0.0f, -1.5f, 5.5f)));
	_camera = camera;
	_shader = shader;
	_position = vec3(0.0f, 0.0f, 5.0f);
	_shot = new Model("assets/objects/starman_ship/shots.obj", _shader);
	timepassedR = timepassedL = 0;
	spotLight = new SpotLight(vec3(0.8f, 0.5f, 0.3f), vec3(0.0f, -1.5f, 10.5f), vec3(1.0f, 0.8f, 0.2f), vec3(0.0f, 0.0f, -1.0f), 25.0f);
	InitPhysicProperties(_position);
}

RPlayer::~RPlayer()
{
}

void RPlayer::InitPhysicProperties(vec3 position)
{
	float numberOfVertices = 0;
	vec3 average(0, 0, 0);

	// convex hull shape
	for (unsigned int i = 0; i < _model->meshes.size(); i++)
	{
		for (unsigned int j = 0; j < _model->meshes.at(i)._vertices.size(); j++)
		{
			shapeVector.push_back(_model->meshes.at(i)._vertices.at(j).Position.x);
			shapeVector.push_back(_model->meshes.at(i)._vertices.at(j).Position.y);
			shapeVector.push_back(_model->meshes.at(i)._vertices.at(j).Position.z);
		}
	}
	_shape = new btConvexHullShape(&shapeVector[0], shapeVector.size() / 3, 3 * sizeof(btScalar));
	//


	// Motion State
	btQuaternion rotationQuat;
	rotationQuat.setEulerZYX(0,0,0);
	btVector3 positionbT = btVector3(position.x, position.y, position.z);
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(rotationQuat, positionbT));
	//

	// Weight
	btScalar mass = 5000;
	btVector3 bodyInertia;
	_shape->calculateLocalInertia(mass, bodyInertia);
	//

	// Rigid Body
	btRigidBody::btRigidBodyConstructionInfo bodyCI = btRigidBody::btRigidBodyConstructionInfo(mass, motionState, _shape, bodyInertia);
	bodyCI.m_restitution = 1.0f;
	bodyCI.m_friction = 0.5f;
	_body = new btRigidBody(bodyCI);
	//

	// Translation & Rotation
	_body->setLinearFactor(btVector3(1, 1, 1));
	//_body->setLinearVelocity(btVector3(0,0,0));

	
}

void RPlayer::addToPhysics()
{
	_world->addRigidBody(_body);
}

void RPlayer::move(float x, float y, bool up, bool down, bool left, bool right, bool shootL, bool shootR, float deltaTime)
{
	btTransform transform = _body->getWorldTransform();
	btQuaternion rota = transform.getRotation();
	//_position = vec3(transform.getOrigin().x(), transform.getOrigin().y(), transform.getOrigin().z());
	//_rotation = vec3(rota.getX(), rota.getY(), rota.getZ());
	
	//////////////////////////////////////////////////////////////////////////
	if (up)
		(_real_speed >= 25) ? _real_speed = 25 : _real_speed += 5 * deltaTime;
	else if (down)
		(_real_speed <= -25) ? _real_speed = -25 : _real_speed -= 5 * deltaTime;

	_speed = (floor((_real_speed * 2) + 0.5) / 2);

	_yaw += _mouse_speed * deltaTime * x;
	_pitch += _mouse_speed * deltaTime * y;

	_dir = vec3(
		cos(_pitch) * sin(_yaw),
		sin(_pitch),
		cos(_pitch) * cos(_yaw)
	);

	_right = vec3(
		sin(_yaw - pi<float>() / 2),
		0,
		cos(_yaw - pi<float>() / 2)
	);

	_up = vec3(glm::cross(_right, _dir));

	_position += _dir * (float)(deltaTime * _speed);

	//_body->setLinearVelocity(btVector3(_dir.x, _dir.y, _dir.z));
	//_body->applyImpulse(btVector3(_dir.x, _dir.y, _dir.z), btVector3(_position.x, _position.y, _position.z));
	//_body->translate(btVector3(_dir.x, _dir.y, _dir.z));
	//_body->applyForce(btVector3(_dir.x, _dir.y, _dir.z), btVector3(_position.x, _position.y, _position.z));

	if (right)
		_position -= _right * (float)(deltaTime * _speed);
	else if (left)
		_position += _right * (float)(deltaTime * _speed);

	_camera->setSpeed(_speed);

	_model->setTransformMatrix(translate(_position) * rotate(_yaw, vec3(0.0f, 1.0f, 0.0f)) * rotate(-_pitch, vec3(1.0f, 0.0f, 0.0f)));

	_camera->update(x, y, up, down, left, right, deltaTime);

	for (int i = 0; i < this->shots.size(); i++)
	{
		this->shots.at(i)->update(deltaTime);
		//collisionCheck(shots.at(i), i);
	}

	for (int i = shots.size() - 1; i >= 0; i--)
	{
		if (shots.at(i)->_toofar /*|| collisionCheck*/)
		{
			_world->removeRigidBody(shots.at(i)->_body);
			shots.erase(shots.begin() + i);
		}
	}

	if(shootL || shootR)
		shoot(deltaTime, shootL, shootR);
}

void RPlayer::shoot(float deltaTime, bool shootL, bool shootR)
{
	timepassedL += deltaTime;
	timepassedR += deltaTime;

	vec3 dir = normalize(_dir);
	vec3 up = normalize(_up);
	vec3 right = normalize(_right);

	if (timepassedL > cooldown && shootL)
	{
		Shots* leftShot1 = new Shots(_shot, _dir, _position + (8.5f*dir - 3.2f*up + 4.5f*right));
		Shots* leftShot2 = new Shots(_shot, _dir, _position + (8.5f*dir - 1.5f*up + 4.5f*right));
		_world->addRigidBody(leftShot1->_body);
		_world->addRigidBody(leftShot2->_body);
		shots.push_back(leftShot1);
		shots.push_back(leftShot2);
		timepassedL = 0;
	}

	if (timepassedR > cooldown && shootR)
	{
		Shots* rightShot1 = new Shots(_shot, _dir, _position + (8.5f*dir - 3.2f*up - 4.5f*right));
		Shots* rightShot2 = new Shots(_shot, _dir, _position + (8.5f*dir - 1.5f*up - 4.5f*right));
		_world->addRigidBody(rightShot1->_body);
		_world->addRigidBody(rightShot2->_body);
		shots.push_back(rightShot1);
		shots.push_back(rightShot2);
		timepassedL = 0;
	}
}

long RPlayer::draw()
{
	long triangles = 0;
	_model->Draw();
	for (int i = 0; i < this->shots.size(); i++)
	{
		if (this->shots.at(i) != nullptr)
			triangles += this->shots.at(i)->draw();
	}
	return triangles;
}

void RPlayer::collisionCheck()
{
	//bool collisionFlag = false;
	int numManifolds = _world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = _world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		//... here you can check for obA�s and obB�s user pointer again to see if the collision is alien and bullet and in that case initiate deletion.
	}

	//return collisionFlag;
}
