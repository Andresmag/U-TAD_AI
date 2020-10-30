#include "stdafx.h"
#include "stateMachine.h"
#include "state.h"
#include "transition.h"
#include "actions/setImageAction.h"
#include "actions/pursueAction.h"
#include "actions/attackAction.h"
#include "actions/wanderAction.h"
#include "conditions/enemyInRangeCondition.h"
#include "conditions/enemyInAttackRangeCondition.h"
#include "conditions/isCharacterAlive.h"
#include "conditions/receivingDamageCondition.h"
#include "conditions/inverseCondition.h"
#include "conditions/andCondition.h"
#include "conditions/orCondition.h"

StateMachine::StateMachine()
    : m_currentState(nullptr)
    , m_Owner(nullptr)
{
}

StateMachine::StateMachine(Character* character)
    : m_currentState(nullptr)
    , m_Owner(character)
{
}

void StateMachine::Load(const char* value)
{
    if(strcmp(value, "player") == 0)
    {
        // Idle State
        State* IdleState = new State(this);
        IdleState->SetEnterAction(new SetImageAction(GetOwner(), 1));

        // Alert State
        State* AlertState = new State(this);
        AlertState->SetEnterAction(new SetImageAction(GetOwner(), 2));
        AlertState->SetStateAction(new PursueAction(GetOwner()));

        // Attack State
        State* AttackState = new State(this);
        AttackState->SetEnterAction(new SetImageAction(GetOwner(), 3));
        AttackState->SetStateAction(new AttackAction(GetOwner(), 50.f));

        // Transitions
        Transition* IdleAlertTransition = new Transition();
        IdleAlertTransition->SetTargetState(AlertState);
        IdleAlertTransition->SetCondition(new AndCondition(new EnemyInRangeCondition(GetOwner()), new IsCharacterAlive(GetOwner()->GetTarget())));

        Transition* IdleAttackTransition = new Transition();
        IdleAttackTransition->SetTargetState(AttackState);
        IdleAttackTransition->SetCondition(new AndCondition(new EnemyInAttackRangeCondition(GetOwner()), new IsCharacterAlive(GetOwner()->GetTarget())));

        Transition* AlertIdleTransition = new Transition();
        AlertIdleTransition->SetTargetState(IdleState);
        AlertIdleTransition->SetCondition(new OrCondition(new InverseCondition(new IsCharacterAlive(GetOwner()->GetTarget())), new InverseCondition(new EnemyInRangeCondition(GetOwner()))));

        Transition* AlertAttackTransition = new Transition();
        AlertAttackTransition->SetTargetState(AttackState);
        AlertAttackTransition->SetCondition(new AndCondition(new EnemyInAttackRangeCondition(GetOwner()), new IsCharacterAlive(GetOwner()->GetTarget())));

        Transition* AttackAlertTransition = new Transition();
        AttackAlertTransition->SetTargetState(AlertState);
        AttackAlertTransition->SetCondition(new InverseCondition(new EnemyInAttackRangeCondition(GetOwner())));

        Transition* AttackIdleTransition = new Transition();
        AttackIdleTransition->SetTargetState(IdleState);
        AttackIdleTransition->SetCondition(new InverseCondition(new IsCharacterAlive(GetOwner()->GetTarget())));

        // Add Transitions to States
        IdleState->AddTransition(IdleAttackTransition); // higher priority
        IdleState->AddTransition(IdleAlertTransition);
        AlertState->AddTransition(AlertIdleTransition);
        AlertState->AddTransition(AlertAttackTransition);
        AttackState->AddTransition(AttackAlertTransition);
        AttackState->AddTransition(AttackIdleTransition);

        // Add States to the State Machine
        m_States.push_back(IdleState);
        m_States.push_back(AlertState);
        m_States.push_back(AttackState);

        m_currentState = m_States.at(0);
    }
    else
    {
        // Idle State
        State* IdleState = new State(this);
        IdleState->SetEnterAction(new SetImageAction(GetOwner(), 0));
        IdleState->SetStateAction(new WanderAction(GetOwner()));

        // Receive damage State
        State* ReceiveDamageState = new State(this);
        ReceiveDamageState->SetEnterAction(new SetImageAction(GetOwner(), 2));
        ReceiveDamageState->SetStateAction(new WanderAction(GetOwner()));

        // Death State
        State* DeathState = new State(this);
        DeathState->SetEnterAction(new SetImageAction(GetOwner(), 3));

        // Transitions
        Transition* IdleDamageTransition = new Transition();
        IdleDamageTransition->SetTargetState(ReceiveDamageState);
        IdleDamageTransition->SetCondition(new ReceivingDamageCondition(GetOwner()));

        Transition* DamageIdleTransition = new Transition();
        DamageIdleTransition->SetTargetState(IdleState);
        DamageIdleTransition->SetCondition(new InverseCondition(new ReceivingDamageCondition(GetOwner())));

        Transition* DamageDeathTransition = new Transition();
        DamageDeathTransition->SetTargetState(DeathState);
        DamageDeathTransition->SetCondition(new InverseCondition(new IsCharacterAlive(GetOwner())));

        // Add Transitions to States
        IdleState->AddTransition(IdleDamageTransition);
        ReceiveDamageState->AddTransition(DamageDeathTransition); // higher priority
        ReceiveDamageState->AddTransition(DamageIdleTransition);

        // Add States to the State Machine
        m_States.push_back(IdleState);
        m_States.push_back(ReceiveDamageState);
        m_States.push_back(DeathState);

        m_currentState = m_States.at(0);
    }

}

void StateMachine::Start()
{
    if (m_currentState)
        m_currentState->OnEnter();
}

void StateMachine::Update(float step)
{
    if (m_currentState)
    {
        m_currentState->Update(step);
        const Transitions& transitions = m_currentState->GetTransitions();
        for (auto transition : transitions)
        {
            if(transition->CanTrigger() && transition->GetTargetState())
            {
                m_currentState->OnExit();
                State* nextState = transition->Trigger();
                nextState->OnEnter();
                m_currentState = nextState;
                return;
            }
        } 
    }
}
