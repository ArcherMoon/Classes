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
    /* ���ñ��� */
    sprite = CCSprite::create("bg-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    this->addChild(sprite, -1);

     /* ����Ͷʯ����*/
    sprite = CCSprite::create("catapult_base_2-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    /* 
   * �����positionʵ�������������Ʒֱ��ʵ�λ�ã�
   * ��������Ƶ���Ƴ����ֱ�����960*640, �����360Ҳ�������960��360��
   * �ڲ�ͬ�ֱ��ʵ��豸�����λ������ͬ�� */
    sprite->setPosition(CCPointMake(360.0f, FLOOR_HEIGHT));
    this->addChild(sprite, 0);

    /* ����������� */
    sprite = CCSprite::create("squirrel_1-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    sprite->setPosition(CCPointMake(30.0f, FLOOR_HEIGHT));
    this->addChild(sprite, 0);

     /* ����Ͷʯ��ǰ��*/
    sprite = CCSprite::create("catapult_base_1-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    sprite->setPosition(CCPointMake(360.0f, FLOOR_HEIGHT));
    this->addChild(sprite, 9);

    /* �����ұ����� */
    sprite = CCSprite::create("squirrel_2-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    sprite->setPosition(CCPointMake(480.0f, FLOOR_HEIGHT));
    this->addChild(sprite, 9);

    /* ����ǰ���������*/
    sprite = CCSprite::create("fg-hd.png");
    sprite->setAnchorPoint(CCPointZero);
    this->addChild(sprite, 10);

    /* ��ʼ���������� */
    /* �������磬�������� */
    b2Vec2 gravtity;
    gravtity.Set(0.0f, -10.0f);
    world = new b2World(gravtity);
    world->SetAllowSleeping(true);

    /* ����������壬
   * ��������(BodyDef)����world����BodyDef����body��������״(shape)��
   * ����о�(FixtureDef)��ָ��shape, ��body����FixtureDef����Fixture */
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
    
    /* ����һ������ */
    /* �������۾��� */
    CCSprite * arm = CCSprite::create("catapult_arm-hd.png");
    this->addChild(arm,  1);

    /* �������۶�Ӧ������body */
    b2BodyDef armBodyDef;
    armBodyDef.type =b2_dynamicBody;
    armBodyDef.linearDamping = 1.0f;
    armBodyDef.angularDamping = 1.0f;
    armBodyDef.position.Set(450.0f/PTM_RATIO, (FLOOR_HEIGHT + 182.0f)/PTM_RATIO);
    armBodyDef.userData = (void *)arm;

    b2Body *armBody = world->CreateBody(&armBodyDef);

    b2PolygonShape armShape;
    /* ����������������ĵ��x��y����ľ��룬������������״ */
    armShape.SetAsBox(11.0f/PTM_RATIO, 182.0f/PTM_RATIO);

    b2FixtureDef armFixDef;
    armFixDef.shape = &armShape;
    armFixDef.density = 0.3f;

    armBody->CreateFixture(&armFixDef);

    /* �������ۺ͵������ת�ؽ� */
    b2RevoluteJointDef armJointDef;
    /* ���ùؽ����ӵ��������弰���ӵ� */
    armJointDef.Initialize(groundBody, armBody, b2Vec2(450.0f/PTM_RATIO, FLOOR_HEIGHT/PTM_RATIO));
    armJointDef.enableLimit = true;                                                /* ������ת�Ƕ� */
    armJointDef.lowerAngle = CC_DEGREES_TO_RADIANS(9);       /* ��С�Ƕ�,�����y��? */
    armJointDef.upperAngle = CC_DEGREES_TO_RADIANS(75);     /* ���Ƕ� */
    armJointDef.enableMotor = true;                                               /* ʹ�������ǵ���? */
    armJointDef.maxMotorTorque = 4800;                                       /* ���Ť�� */
    armJointDef.motorSpeed = -10;                                               /* ����ٶ� */
    armJoint = (b2RevoluteJoint *)world->CreateJoint(&armJointDef);

    /* ���ö�ʱ������ʱ�������������step��ͬʱ�����������body����cocos2d�ľ��� */
    this->schedule(schedule_selector(HelloWorld::tick));
    
    return true;
}

void HelloWorld::tick(float dt)
{
    /* �������������step */
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
