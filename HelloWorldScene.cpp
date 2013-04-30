#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#define FLOOR_HEIGHT 120.0f
#define PTM_RATIO 32

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback) );
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    CCSprite *sprite = NULL;
    /* 设置背景 */
    sprite = CCSprite::create("bg-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    this->addChild(sprite, -1);

     /* 设置投石器后部*/
    sprite = CCSprite::create("catapult_base_2-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    /* 
   * 这里的position实际上是相对于设计分辨率的位置，
   * 本例中设计的设计场景分辨率是960*640, 下面的360也是相对于960的360，
   * 在不同分辨率的设备上相对位置是相同的 */
    sprite->setPosition(CCPointMake(360.0f, FLOOR_HEIGHT));
    this->addChild(sprite, 0);

    /* 设置左边松鼠 */
    sprite = CCSprite::create("squirrel_1-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    sprite->setPosition(CCPointMake(30.0f, FLOOR_HEIGHT));
    this->addChild(sprite, 0);

     /* 设置投石器前部*/
    sprite = CCSprite::create("catapult_base_1-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    sprite->setPosition(CCPointMake(360.0f, FLOOR_HEIGHT));
    this->addChild(sprite, 9);

    /* 设置右边松鼠 */
    sprite = CCSprite::create("squirrel_2-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    sprite->setPosition(CCPointMake(480.0f, FLOOR_HEIGHT));
    this->addChild(sprite, 9);

    /* 设置前面带冰地面*/
    sprite = CCSprite::create("fg-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    this->addChild(sprite, 10);

    /* 初始化物理世界 */
    /* 创造世界，设置重力 */
    b2Vec2 gravtity;
    gravtity.Set(0.0f, -10.0f);
    world = new b2World(gravtity);
    world->SetAllowSleeping(true);

    /* 创造地面物体，
   * 定义物体(BodyDef)，由world根据BodyDef创造body，创造形状(shape)，
   * 定义夹具(FixtureDef)，指向shape, 由body根据FixtureDef创造Fixture */
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0,0);

    b2Body * groundBody = world->CreateBody(&groundBodyDef);

    b2EdgeShape groundBox;

    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    /* bottom */
    groundBox.Set(b2Vec2(0, FLOOR_HEIGHT/PTM_RATIO), 
                          b2Vec2(screenSize.width * 2.0f/PTM_RATIO, FLOOR_HEIGHT/PTM_RATIO));
    groundBody->CreateFixture(&groundBox,0);

     /* top */
    groundBox.Set(b2Vec2(0, screenSize.height/PTM_RATIO), 
                          b2Vec2(screenSize.width * 2.0f/PTM_RATIO, screenSize.height/PTM_RATIO));
    groundBody->CreateFixture(&groundBox,0);
    
     /* left */
    groundBox.Set(b2Vec2(0, 0), b2Vec2(0, screenSize.height/PTM_RATIO));
    groundBody->CreateFixture(&groundBox,0);

     /* right */
    groundBox.Set(b2Vec2(screenSize.width * 2.0f/PTM_RATIO,  0),
                          b2Vec2(screenSize.width * 2.0f/PTM_RATIO,  screenSize.height/PTM_RATIO));
    groundBody->CreateFixture(&groundBox,0);    
    
    /* 创造一般物体 */
    /* 创建弹臂精灵 */
    CCSprite * arm = CCSprite::create("catapult_arm-hd.png");
    this->addChild(arm,  1);

    /* 创建弹臂对应的物理body */
    b2BodyDef armBodyDef;
    armBodyDef.type =b2_dynamicBody;
    armBodyDef.linearDamping = 1.0f;
    armBodyDef.angularDamping = 1.0f;
    armBodyDef.position.Set(450.0f/PTM_RATIO, (FLOOR_HEIGHT + 182.0f)/PTM_RATIO);
    armBodyDef.userData = (void *)arm;

    b2Body *armBody = world->CreateBody(&armBodyDef);

    b2PolygonShape armShape;
    /* 设置相对于物体中心点的x，y方向的距离，生成真正的形状 */
    armShape.SetAsBox(11.0f/PTM_RATIO, 182.0f/PTM_RATIO);

    b2FixtureDef armFixDef;
    armFixDef.shape = &armShape;
    armFixDef.density = 0.3f;

    armBody->CreateFixture(&armFixDef);

    /* 创建弹臂和地面的旋转关节 */
    b2RevoluteJointDef armJointDef;
    /* 设置关节链接的两个物体及连接点 */
    armJointDef.Initialize(groundBody, armBody, b2Vec2(450.0f/PTM_RATIO, FLOOR_HEIGHT/PTM_RATIO));
    armJointDef.enableLimit = true;                                                /* 限制旋转角度 */
    armJointDef.lowerAngle = CC_DEGREES_TO_RADIANS(9);       /* 最小角度,相对于y轴? */
    armJointDef.upperAngle = CC_DEGREES_TO_RADIANS(75);     /* 最大角度 */
    armJointDef.enableMotor = true;                                               /* 使能马达，或是弹簧? */
    armJointDef.maxMotorTorque = 4800;                                       /* 最大扭矩 */
    armJointDef.motorSpeed = -10;                                               /* 马达速度 */
    armJoint = (b2RevoluteJoint *)world->CreateJoint(&armJointDef);

    /* 设置定时器，定时更新物理世界的step，同时由物理世界的body更新cocos2d的精灵 */
    this->schedule(schedule_selector(HelloWorld::tick));
    
    return true;
}

void HelloWorld::tick(float dt)
{
    /* 更新物理世界的step */
    int velocityIterations = 8;
    int positionIterations = 1;
    world->Step(dt, velocityIterations,  positionIterations);

    CCSprite * sprite;
    for (b2Body *body = world->GetBodyList(); NULL != body; body = body->GetNext())
    {
        if (NULL != body->GetUserData())
        {
            sprite = (CCSprite *)body->GetUserData();
            sprite->setPosition(CCPointMake(body->GetPosition().x * PTM_RATIO, body->GetPosition().y * PTM_RATIO));
            sprite->setRotation(-1 * CC_RADIANS_TO_DEGREES(body->GetAngle()));
        }
    }
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
