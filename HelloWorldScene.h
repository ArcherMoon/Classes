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

    /* 定时器回调函数 */
    void tick(float dt );

    /* 实现Layer的触摸开始，移动，结束方法 */
    void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);

    /* 创建松果 */
    void createBullets(int count);
    /* 将松果炮弹与弹臂焊接起来 */
    bool attachBullet();
    /* 创建单个目标 */
    void createTarget(const char *imageName,
                               cocos2d::CCPoint position,
                               float rotation,
                               bool isCircle,
                               bool isStatic,
                               bool isEnemy);
    /* 创建一堆目标 */
    void createTarges();

    /* 从新装载炮弹并将画面移到最左边 */
    void resetBullet();
    /* 重置游戏 */
    void resetGame();

private:
    b2World *world;                  /* 物理世界 */
    b2Body * groundBody;
    b2Body *armBody;
    b2RevoluteJoint *armJoint;          /* 弹臂与地面的旋转关节 */
    b2MouseJoint *mouseJoint;        /* 鼠标关节，将物体移动到指定点 */
    b2WeldJoint *bulletJoint;           /* 炮弹和弹臂的焊接关节 */
    b2Body * bulletBody;                 /* 当前的松果炮弹 */
    std::vector<b2Body *> bullets;   /* 炮弹数组 */
    int currentBullet;                      /* 当前松果序号 */
    bool releasingArm;                  /* 判断弹臂是否释放 */      
    std::vector<b2Body *> targets;       /* 打击目标，包括木板和敌人 */ 
    std::vector<b2Body *> enemies;   /* 打击的敌人 */ 
    MyContactListener * contactListener;    /* 创建碰撞监听器指针 */
};

#endif // __HELLOWORLD_SCENE_H__
