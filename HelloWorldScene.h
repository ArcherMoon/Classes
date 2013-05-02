#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "MyContactListener.h"

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
    /* ���ɹ��ڵ��뵯�ۺ������� */
    bool attachBullet();
    /* ��������Ŀ�� */
    void createTarget(const char *imageName,
                               cocos2d::CCPoint position,
                               float rotation,
                               bool isCircle,
                               bool isStatic,
                               bool isEnemy);
    /* ����һ��Ŀ�� */
    void createTarges();

    /* ����װ���ڵ����������Ƶ������ */
    void resetBullet();
    /* ������Ϸ */
    void resetGame();

private:
    b2World *world;                  /* �������� */
    b2Body * groundBody;
    b2Body *armBody;
    b2RevoluteJoint *armJoint;          /* ������������ת�ؽ� */
    b2MouseJoint *mouseJoint;        /* ���ؽڣ��������ƶ���ָ���� */
    b2WeldJoint *bulletJoint;           /* �ڵ��͵��۵ĺ��ӹؽ� */
    b2Body * bulletBody;                 /* ��ǰ���ɹ��ڵ� */
    std::vector<b2Body *> bullets;   /* �ڵ����� */
    int currentBullet;                      /* ��ǰ�ɹ���� */
    bool releasingArm;                  /* �жϵ����Ƿ��ͷ� */      
    std::vector<b2Body *> targets;       /* ���Ŀ�꣬����ľ��͵��� */ 
    std::vector<b2Body *> enemies;   /* ����ĵ��� */ 
    MyContactListener * contactListener;    /* ������ײ������ָ�� */
};

#endif // __HELLOWORLD_SCENE_H__
