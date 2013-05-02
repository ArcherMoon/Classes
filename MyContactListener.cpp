#include "MyContactListener.h"

/* contact��ʼ��? */
MyContactListener::MyContactListener(): contacts()
{
}

MyContactListener::~MyContactListener()
{
}

void MyContactListener::BeginContact(b2Contact* contact)
{
}

void MyContactListener::EndContact(b2Contact* contact)
{
}

void MyContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
}

/* contact������ײ�������оߣ�impulse�洢���������о�ÿ����ײ��ĳ��� */
void MyContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    /* ȡ�������о�,�ж��Ƿ��ǵ��� */
    bool isAEnemy = contact->GetFixtureA()->GetUserData() != NULL;
    bool isBEnemy = contact->GetFixtureB()->GetUserData() != NULL;

    /* �������������һ���ǵ�������������жϴ��� */
    if (isAEnemy || isBEnemy)
    {
        /* ����ÿ����ײ��ĳ���������������ĳ��������������򽫵��˼������ٵļ��� */
        /* ȡ��������ײ�� */
        int32 count = contact->GetManifold()->pointCount;
        /* ȡ�������� */
        float32 maxImpulse = 0.0f;
        for (int32 i = 0; i < count; ++i)
        {
            maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]);
        }

        /* �������������� */
        if (maxImpulse > 1.0f)
        {
            if (isAEnemy)    
            {
                contacts.insert(contact->GetFixtureA()->GetBody());
            }
            if (isBEnemy)    
            {
                contacts.insert(contact->GetFixtureB()->GetBody());
            }
        }
    }
}