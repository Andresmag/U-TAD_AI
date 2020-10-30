#pragma once

#include "action.h"
#include "../../character.h"

class SetImageAction : public Action
{
    int m_imageID;

public:
    SetImageAction(Character* owner, int imageID)
        : m_imageID(imageID)
    {
        m_OwnerCharacter = owner;
    }

    void Start() override
    {
        if (m_OwnerCharacter)
        {
            m_OwnerCharacter->SetImage(m_imageID);
        }
    }
};