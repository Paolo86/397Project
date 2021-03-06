#pragma once
#include <string>

#include "..\GameObject\Component.h"
#include "Animator.h"
#include "..\Core\Timer.h"
#include "..\Core\Lua.h"
#include "..\Utils\PathFinder.h"
#include "..\Prefabs\Terrain.h"

class AIBase : public Component
{
public:
	/**
	* @brief		Constructor
	*
	* @pre			The spider object does not exist
	* @post			The spider object is created
	*
	* @param		scriptPath	path to the ai script
	*/
	AIBase(std::string scriptPath); //Note: do not call Update function without setting a target first!

	/**
	* @brief		Constructor
	*
	* @pre			The aibase object does not exist
	* @post			The aibase object is created
	*
	* @param		targetTransform	transform of the target
	* @param		scriptPath	path to the ai script
	*/
	AIBase(Transform& targetTransform, std::string scriptPath);

	/**
	* @brief		Destructor
	*
	* @pre			The aibase object must exist
	* @post			The aibase object is destroyed
	*/
	~AIBase();

	/**
	* @brief		Sets the target for the aibase
	*
	* @pre			The aibase object must exist
	*
	* @param		targetTransform	transform of the target
	*/
	void SetTarget(Transform& targetTransform);

	/**
	* @brief		Returns the distance to the target transform
	*
	* @pre			The aibase object must exist
	*
	* @return		distance to the target transform
	*/
	float GetDistanceToTarget() const;

	/**
	* @brief		Returns the rotation to the target transform
	*
	* @pre			The aibase object must exist
	*
	* @return		rotation to the target transform
	*/
	float GetRotationToTarget() const;

	/**
	* @brief		Returns the reversed rotation to the target transform
	*
	* @pre			The aibase object must exist
	*
	* @return		reversed rotation to the target transform
	*/
	float GetReverseRotationToTarget() const; //Same as above but reversed by 180 degrees

	/**
	* @brief		Returns the distance to the pathfinding node
	*
	* @pre			The aibase object must exist
	*
	* @return		distance to the pathfinding node
	*/
	float GetDistanceToNode() const;

	/**
	* @brief		Returns the rotation to the pathfinding node
	*
	* @pre			The aibase object must exist
	*
	* @return		rotation to the pathfinding node
	*/
	float GetRotationToNode() const;

	/**
	* @brief		Returns the reversed rotation to the pathfinding node
	*
	* @pre			The aibase object must exist
	*
	* @return		reversed rotation to the pathfinding node
	*/
	float GetReverseRotationToNode() const; //Same as above but reversed by 180 degrees

	/**
	* @brief		Returns the aibase's target transform
	*
	* @pre			The aibase object must exist
	*
	* @return		the aibase's target transform
	*/
	Transform* GetTarget() const;

	/**
	* @brief		Sets the ai state
	*
	* @pre			The aibase object must exist
	*
	* @param		state	the ai state
	*/
	void SetState(std::string state);

	/**
	* @brief		Gets the ai state
	*
	* @pre			The aibase object must exist
	*
	* @return		the ai state
	*/
	std::string GetState() const;

	/**
	* @brief		Sets whether the ai has received an event
	*
	* @pre			The aibase object must exist
	*
	* @param		whether the ai has received an event
	*/
	void SetEventReceived(bool received);

	/**
	* @brief		Gets whether the ai has received an event
	*
	* @pre			The aibase object must exist
	*
	* @return		whether the ai has received an event
	*/
	bool GetEventReceived() const;

	/**
	* @brief		 Moves the transform by the specified amount
	*
	* @pre			The aibase object must exist
	*
	* @param		forward	amount to move forward
	* @param		right	amount to move right
	*/
	void Move(float forward = 0.0f, float right = 0.0f);

	/**
	* @brief		 Moves the transform forward by the specified amount
	*
	* @pre			The aibase object must exist
	*
	* @param		forward	amount to move forward
	*/
	void MoveForward(float forward = 0.0f);

	/**
	* @brief		 Moves the transform right by the specified amount
	*
	* @pre			The aibase object must exist
	*
	* @param		right	amount to move right
	*/
	void MoveRight(float right = 0.0f);

	/**
	* @brief		Rotates the transform right by the specified amount
	*
	* @pre			The aibase object must exist
	*
	* @param		amount	amount to rotate
	*/
	void Rotate(float amount = 0.0f);

	/**
	* @brief		Sets the animation of the parent
	*
	* @pre			The aibase object must exist
	*
	* @param		index	index of the animation
	*/
	void SetAnimation(int index = 0);

	/**
	* @brief		Overridden callback for update method
	*
	* @pre			The aibase object must exist
	*/
	void Update() override;

	/**
	* @brief		Overridden callback for onattach method
	*
	* @pre			The aibase object must exist
	*/
	virtual void OnAttach(GameObject* go) override;

	/**
	* @brief		dumps all the aibase variables to the logger
	*/
	void DumpVariables();

private:
	std::string _state;
	Transform* _parentTransform;
	Transform* _targetTransform;
	lua_State* _luaState;
	glm::vec3 _otherTarget; //Postition that can be used for wandering/patrolling or any other location based logic
	glm::vec3 _nextNode;
	bool _eventReceived;

	std::string _scriptPath;

	float _lastStateChange; //Time since the last succesful state change
	float _randomTimer; //Timer for managing random state changes
	float _updateTimer;

	//AI Functions
	/**
	* @brief		Calls the think() function in the given lua script
	*/
	void Think();
	//----------
};

