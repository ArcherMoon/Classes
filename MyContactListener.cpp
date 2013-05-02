#include "MyContactListener.h"

/* contact初始化? */
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

/* contact中有碰撞的两个夹具，impulse存储了这两个夹具每个碰撞点的冲力 */
void MyContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    /* 取得两个夹具,判断是否是敌人 */
    bool isAEnemy = contact->GetFixtureA()->GetUserData() != NULL;
    bool isBEnemy = contact->GetFixtureB()->GetUserData() != NULL;

    /* 如果其中至少有一个是敌人则进行销毁判断处理 */
    if (isAEnemy || isBEnemy)
    {
        /* 遍历每个碰撞点的冲力，如果其中最大的冲力满足条件，则将敌人加入销毁的集合 */
        /* 取得所有碰撞点 */
        int32 count = contact->GetManifold()->pointCount;
        /* 取得最大冲力 */
        float32 maxImpulse = 0.0f;
        for (int32 i = 0; i < count; ++i)
        {
            maxImpulse = b2Max(maxImpulse, impulse->normalImpulses[i]);
        }

        /* 最大冲力满足条件 */
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