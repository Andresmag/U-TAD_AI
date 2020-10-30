#include "stdafx.h"
#include "changeImageBehavior.h"
#include "character.h"
#include "../status.h"

ChangeImageBehavior::ChangeImageBehavior(Character* owner, int index)
    : m_ImagenIndex(index)
{
    SetOwner(owner);
}

void ChangeImageBehavior::OnEnter()
{
    GetOwner()->SetImage(m_ImagenIndex);
}

Status ChangeImageBehavior::Update(float step)
{
    return Status::eSuccess;
}
