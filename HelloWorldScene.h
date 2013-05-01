#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

    /* ��ʱ���ص����� */
    void tick(float dt );

    /* ʵ��Layer�Ĵ�����ʼ���ƶ����������� */
    void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);

    /* �����ɹ� */
    void createBullets(int count);

private:
    b2World *world;                  /* �������� */
    b2Body * groundBody;
    b2Body *armBody;
    b2RevoluteJoint *armJoint;  /* ������������ת�ؽ� */
    b2MouseJoint *mouseJoint;   /* ���ؽڣ��������ƶ���ָ���� */
    b2Body * bullet;                    /* �ɹ��ڵ� */
    std::vector<b2Body *> bullets;   /* �ڵ����� */
    int currentBullet;                       /* ��ǰ�ɹ���� */
};

#endif // __HELLOWORLD_SCENE_H__
