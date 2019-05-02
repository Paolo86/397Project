#include "AIBase.h"

AIBase::AIBase(Transform& targetTransform, AIState state) : Component("AIBase")
{
	_type = "AI";
	SetState(state);
	//_wanderDirection = RandUtils::RandFloat(0, 360);
	SetMovementSpeed(100.0f);
	SetRotationSpeed(1.0f);
	_lastStateChange = 0.0f;
	SetTarget(targetTransform);
	SetAgroDistance(500.0f);
	SetMaxFollowDistance(1000.0f);
	SetFleeDistance(1000.0f);
	SetAttackDistance(150.0f);
}

AIBase::~AIBase()
{

}

void AIBase::SetTarget(Transform& targetTransform)
{
	_targetTransform = &targetTransform;
}

float AIBase::GetDistanceToTarget() const
{
	return sqrt(pow(_targetTransform->GetPosition().x - _parentTransform->GetPosition().x, 2) + pow(_targetTransform->GetPosition().y - _parentTransform->GetPosition().y, 2) + pow(_targetTransform->GetPosition().z - _parentTransform->GetPosition().z, 2));
}

float AIBase::GetRotationToTarget() const
{
	glm::vec3 toTarget = glm::vec3(_targetTransform->GetPosition().x, _parentTransform->GetPosition().y, _targetTransform->GetPosition().z) - _parentTransform->GetPosition();
	float yAngle = glm::degrees(glm::angle(_parentTransform->GetLocalFront(), glm::normalize(toTarget))) - 180;
	glm::vec3 cross = glm::normalize(glm::cross(_parentTransform->GetLocalFront(), glm::normalize(toTarget)));
	int s = glm::sign(cross.y);
	return yAngle * s;
}

Transform* AIBase::GetTarget() const
{
	return _targetTransform;
}

void AIBase::SetState(AIState state)
{
	_state = state;
	_lastStateChange = Timer::GetTimeS();
}

AIState AIBase::GetState() const
{
	return _state;
}

void AIBase::SetMovementSpeed(float movementSpeed)
{
	if (movementSpeed >= 0)
	{
		_movementSpeed = movementSpeed;
	}
}

float AIBase::GetMovementSpeed() const
{
	return _movementSpeed;
}

void AIBase::SetRotationSpeed(float rotationSpeed)
{
	if (rotationSpeed >= 0)
	{
		_rotationSpeed = rotationSpeed;
	}
}

float AIBase::GetRotationSpeed() const
{
	return _rotationSpeed;
}

void AIBase::SetFleeDistance(float fleeDistance)
{
	if (fleeDistance >= 0)
	{
		_fleeDistance = fleeDistance;
	}
}

float AIBase::GetFleeDistance() const
{
	return _fleeDistance;
}

void AIBase::SetAgroDistance(float agroDistance)
{
	if (agroDistance >= 0)
	{
		_agroDistance = agroDistance;
	}
}

float AIBase::GetAgroDistance() const
{
	return _agroDistance;
}

void AIBase::SetMaxFollowDistance(float maxFollowDistance)
{
	if (maxFollowDistance >= 0)
	{
		_maxFollowDistance = maxFollowDistance;
	}
}

float AIBase::GetMaxFollowDistance() const
{
	return _maxFollowDistance;
}

void AIBase::SetAttackDistance(float attackDistance)
{
	if (attackDistance >= 0)
	{
		_attackDistance = attackDistance;
	}
}

float AIBase::GetAttackDistance() const
{
	return _attackDistance;
}

void AIBase::Update()
{
	Think();
}

void AIBase::OnAttach(GameObject* go)
{
	_parentTransform = &go->transform;
}

void AIBase::Think()
{
	switch (_state)
	{
		case AIState::Idle:
			Idle();
			break;
		case AIState::Wander:
			Wander();
			break;
		case AIState::Seek:
			Seek();
			break;
		case AIState::Fight:
			Fight();
			break;
		case AIState::Flee:
			Flee();
			break;
		default:
			Logger::LogError("Invalid AI State");
			throw "Invalid_AI_State";
			break;
	}
}

void AIBase::Idle()
{
	GetParent()->GetComponent<Animator>("Animator")->SetCurrentAnimation(14);

	if (Timer::GetTimeS() - _randomTimer > 0.5f && Timer::GetTimeS() - _lastStateChange > 5.0f)
	{
		_randomTimer = Timer::GetTimeS();

		if (RandUtils::RandInt(1, 100) < 10)
		{
			Logger::LogInfo("Now Wandering");
			SetState(AIState::Wander);
		}

		if (GetDistanceToTarget() <= GetAgroDistance())
		{
			Logger::LogInfo("Now Seeking");
			SetState(AIState::Seek);
		}
	}
}

void AIBase::Wander()
{
	GetParent()->GetComponent<Animator>("Animator")->SetCurrentAnimation(7);

	float rotateAmount = 0;

	if (RandUtils::RandInt(1, 100) <= 10)
	{
		rotateAmount += RandUtils::RandFloat(-10, 0);
	}
	else if (RandUtils::RandInt(1, 100) >= 90)
	{
		rotateAmount += RandUtils::RandFloat(-0, 10);
	}

	_parentTransform->RotateBy(rotateAmount * GetRotationSpeed() * Timer::GetDeltaS(), 0, 1, 0);
	//_parentTransform->RotateBy(180 * GetRotationSpeed() * Timer::GetDeltaS(), 0, 1, 0);
	_parentTransform->SetPosition(_parentTransform->GetPosition() - (GetMovementSpeed() * Timer::GetDeltaS() * _parentTransform->GetLocalFront()));

	if (Timer::GetTimeS() - _randomTimer > 0.5f && Timer::GetTimeS() - _lastStateChange > 5.0f)
	{
		_randomTimer = Timer::GetTimeS();

		if (RandUtils::RandInt(1, 100) < 10)
		{
			Logger::LogInfo("Now Idle");
			SetState(AIState::Idle);
		}

		if (GetDistanceToTarget() <= GetAgroDistance())
		{
			Logger::LogInfo("Now Seeking");
			SetState(AIState::Seek);
		}

	}
}

void AIBase::Seek()
{
	GetParent()->GetComponent<Animator>("Animator")->SetCurrentAnimation(7);

	float rotation = GetRotationToTarget() * GetRotationSpeed() * Timer::GetDeltaS();
	//_wanderDirection = rotation;
	_parentTransform->RotateBy(rotation, 0, 1, 0);
	//_parentTransform->RotateBy(180 * GetRotationSpeed() * Timer::GetDeltaS(), 0, 1, 0);
	_parentTransform->SetPosition(_parentTransform->GetPosition() - (GetMovementSpeed() * Timer::GetDeltaS() * _parentTransform->GetLocalFront()));

	if (Timer::GetTimeS() - _randomTimer > 0.5f && Timer::GetTimeS() - _lastStateChange > 5.0f)
	{
		_randomTimer = Timer::GetTimeS();

		if (GetDistanceToTarget() >= GetMaxFollowDistance())
		{
			Logger::LogInfo("Returning to Wander");
			SetState(AIState::Wander);
		}
		else if (GetDistanceToTarget() <= GetAttackDistance())
		{
			Logger::LogInfo("Now Fighting");
			SetState(AIState::Fight);
		}
	}
}

void AIBase::Fight()
{
	GetParent()->GetComponent<Animator>("Animator")->SetCurrentAnimation(0);

	if (Timer::GetTimeS() - _randomTimer > 0.5f && Timer::GetTimeS() - _lastStateChange > 5.0f)
	{
		_randomTimer = Timer::GetTimeS();

		if (GetDistanceToTarget() > GetAttackDistance())
		{
			Logger::LogInfo("Now Seeking");
			SetState(AIState::Seek);
		}
	}
}

void AIBase::Flee()
{
	//Pick the opposite direction to the target and run until you are x distance away
}