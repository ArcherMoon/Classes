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
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 70) );

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 11);

    /////////////////////////////
    // 3. add your codes below...

    /* ���۴���δ�ͷ�״̬ */
    releasingArm = false;

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
    world->SetContinuousPhysics(true);

    /* ����������壬
   * ��������(BodyDef)����world����BodyDef����body��������״(shape)��
   * ����о�(FixtureDef)��ָ��shape, ��body����FixtureDef����Fixture */
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0,0);

    groundBody = world->CreateBody(&groundBodyDef);

    b2EdgeShape groundBox;

    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    CCLOG("screenWidth = %f, screenHeight = %f.", screenSize.width, screenSize.height);
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

    armBody = world->CreateBody(&armBodyDef);

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
    armJointDef.maxMotorTorque = 1500;                                       /* ���Ť�أ����ת������ҪŤ�����ģ�Խ���򷴵���Խ���׳�ʱ���ٶ�Խ�� */
    armJointDef.motorSpeed = -10;                                               /* ����ٶ�,1s��ת���ٶȣ������� */
    armJoint = (b2RevoluteJoint *)world->CreateJoint(&armJointDef);   

    /* ��ʼ�����ؽ�ָ��Ϊ�գ�ʹ�ܴ��� */
    mouseJoint = NULL;
    this->setTouchEnabled(true);

    /* ������Ϸ,Ԥ�����ڵ�����ƫ9�ȵķ���󶨣�����Ϸ�տ�ʼʱ��
   * ���ۻ�������Y��0�ȵ�λ�ã���Ҫ����һ�£���һ�ᣬ�ȵ����۵�ƫ9�ȵ�λ���ٰ� */
   CCFiniteTimeAction *action = CCSequence::create(
            CCDelayTime::create(0.2f),
            CCCallFunc::create(this, callfunc_selector(HelloWorld::resetGame)),
            NULL);
    this->runAction(action);

    /* ������ײ������������������� */
    contactListener = new MyContactListener();  /* ע������ */
    world->SetContactListener(contactListener);
    
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

    /* ������۴����ͷ�״̬��С��һ���Ƕȣ���ɾ�����ӹؽ� */
    if (releasingArm && NULL != bulletJoint)
    {
            if (armJoint->GetJointAngle() <= CC_DEGREES_TO_RADIANS(30))
            {
                releasingArm =false;
                world->DestroyJoint(bulletJoint);
                bulletJoint = NULL;

                /* �ڵ��������󣬽������ƻ�����߲�����װ���ڵ� */
                /* layer��ִ����ʱ��������ִ��װ���ڵ����� */
                CCFiniteTimeAction * action = CCSequence::create(
                        CCDelayTime::create(5.0f),
                        CCCallFunc::create(this, callfunc_selector(HelloWorld::resetBullet)),
                        NULL);
                this->runAction(action);
            }
    }

    /* �ж��ڵ��Ƿ�����Ļ���ĵ���ұߣ��ƶ��ӽ� */
    if (NULL != bulletBody && NULL == bulletJoint)
    {
        /* ȡ���ڵ����ĵ� */
        b2Vec2 position = bulletBody->GetPosition();
        /* ȡ��layer������,��ccNode�̳�getPosition */
        CCPoint myPosition = this->getPosition();
        /* ȡ����Ļ��� */
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

        if (position.x > screenSize.width /2.0f /PTM_RATIO)
        {
             /* ����layer��������,��ccNode�̳�setPosition */
             myPosition.x = -MIN(screenSize.width * 2.0f - screenSize.width, position.x * PTM_RATIO - screenSize.width /2.0f);
             this->setPosition(myPosition);
        }   
    }

    /* --������ײ�ĵ���-- */
    /* ����������������contacts���� */
    std::set<b2Body *>::iterator pos;
    for (pos = contactListener->contacts.begin();
           pos != contactListener->contacts.end();
           ++pos)
    {
        /* ȡ��ָ��ָ������ݣ���b2Body* */
        b2Body *body = *pos;

        /* ��targes��ɾ�� */
        for (vector<b2Body *>::iterator iter = targets.begin();
                iter != targets.end();
                ++iter)
        {
            if (body == *iter)
            {
                /* ����ɾ��Ԫ�ص���һ��Ԫ��ָ�� */
                iter = targets.erase(iter);
                break;
            }
        }   

        /* ��enemies��ɾ�� */
        for (vector<b2Body *>::iterator iter = enemies.begin();
                iter != enemies.end();
                ++iter)
        {
            if (body == *iter)
            {
                iter = enemies.erase(iter);
                break;
            }
        }        

        /* ɾ��layer�ж�Ӧ�ľ��� */
        CCNode *contactNode = (CCNode *)body->GetUserData();
        CCPoint position = contactNode->getPosition();
        this->removeChild(contactNode);

        /* ɾ��world�е�body */
        world->DestroyBody(body);

        /* --��������Ч��-- */
        CCParticleSun *explosion = CCParticleSun::create();
        /* �������� */
        explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire.png"));
        /* һЩ���� */
        explosion->retain(); 
        explosion->initWithTotalParticles(200); 
        explosion->setAutoRemoveOnFinish(true); 
        explosion->setStartSizeVar(10.0f); 
        explosion->setSpeed(70.0f); 
        explosion->setAnchorPoint(CCPointMake(0.5f, 0.5f)); 
        explosion->setPosition(position); 
        explosion->setDuration(1.0f); 
        this->addChild(explosion, 11);
    }
    /* ���contacts�����ڱ����´���ײ��Ҫ�ݻٵĵ��� */
    contactListener->contacts.clear();
    
}

void HelloWorld::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    /* ����Ѿ������ؽ�˵�������ƶ��У����� */
    if (NULL != mouseJoint)
    {
        return;
    }
    
    /* ȡ����һtouch */
    CCTouch *touch = (CCTouch *)pTouches->anyObject();
    /* ��ȡ���� */
    CCPoint location = touch->getLocation();
    /* ת��Ϊbox2d���� */
    b2Vec2 target = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);

    /* �������ؽ� */
    if (target.x < armBody->GetWorldCenter().x + 50.0f/PTM_RATIO)
    {
        b2MouseJointDef mouceJointDef;
        /* �����bodyA,bodyB�ǽṹ��b2MouseJointDef��b2JointDef�̳ж��� */
        mouceJointDef.bodyA = groundBody;
        mouceJointDef.bodyB = armBody;
        mouceJointDef.target = target;
        mouceJointDef.maxForce = 2000;  /* ��С�����������ۣ���Ӱ�쵯�۵ķ����� */
        mouseJoint = (b2MouseJoint *)world->CreateJoint(&mouceJointDef);
    }
}

void HelloWorld::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    /* ���û�����ؽ�ֱ�ӷ��� */
    if (NULL == mouseJoint)
    {
        return;
    }
    
    /* ȡ����һtouch */
    CCTouch *touch = (CCTouch *)pTouches->anyObject();
    /* ��ȡ���� */
    CCPoint location = touch->getLocation();
    /* ת��Ϊbox2d���� */
    b2Vec2 target = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);

    /* �޸����ؽ�target��ֵ */
    mouseJoint->SetTarget(target);
    return;
}

void HelloWorld::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    /* �������ؽ� */
    if (NULL != mouseJoint)
    {
        if (armJoint->GetJointAngle() > CC_DEGREES_TO_RADIANS(20))
        {
            releasingArm = true;
        }
        world->DestroyJoint(mouseJoint);
        mouseJoint = NULL;
    }
    return;       
}

void HelloWorld::createBullets(int count)
{
    if (count <= 0)
    {
        return;
    }
    /* ��ǰ�ɹ��������е���� */
    currentBullet = 0;
    /* ��һ���ɹ��İڷ�λ�ã�ע��Ϊ��������ĵ� */
    float pos = 140.0f;
    float delta = 0.0f;

    /*  ������������+����ɹ�����̨-����ɹ�֮��ڷ������ɹ��ļ�� */
    if (count > 1)
    {
        delta = (320.0f - 30.0f - pos) / (count - 1);
    }

    /* ѭ�������ɹ�body���������� */
    CCSprite *sprite = NULL;
    b2Body * bullet = NULL;
    for (int i=0; i < count; i++, pos += delta)
    {
        sprite = CCSprite::create("acorn-hd.png");
        this->addChild(sprite, 1);

        b2BodyDef bulletBodyDef;
        bulletBodyDef.type = b2_dynamicBody;
        bulletBodyDef.bullet = true;  /* �����������������ײ���(CCD)���� */
        bulletBodyDef.position.Set(pos/PTM_RATIO, (FLOOR_HEIGHT + 28.0f)/PTM_RATIO);
        bulletBodyDef.userData = (void *)sprite;
        bullet = world->CreateBody(&bulletBodyDef);
        bullet->SetActive(false);       /* ��Ȼ�Ƕ�̬���壬�Ȳ�������Ч�� */

        b2CircleShape circle;
        circle.m_radius = 30.0f/PTM_RATIO;

        b2FixtureDef bulletFixDef;
        bulletFixDef.shape = &circle;
        bulletFixDef.density = 0.8f;
        bulletFixDef.friction = 0.99f;
        bulletFixDef.restitution = 0.2f;
        bullet->CreateFixture(&bulletFixDef);
        
        bullets.push_back(bullet);      
    }
    return;
}

bool HelloWorld::attachBullet()
{
    /* �����ǰ�ڵ�����Ŵ��ڵ����������򷵻� */
    if (currentBullet >= bullets.size())
    {
        return false;
    }
    
    /* �ɵ�ǰ�ڵ�����ҵ���ǰ�ڵ� */
    bulletBody = bullets.at(currentBullet++);
    /* �ƶ������۴� */
    bulletBody->SetTransform(b2Vec2(450.0f/PTM_RATIO, (FLOOR_HEIGHT + 310.0f)/PTM_RATIO), 0.0f);
    /* ���������溸��û��Ч�� */
    bulletBody->SetActive(true);

    /* �������ӹؽڣ����ڵ��뵯�ۺ�����һ�� */
    b2WeldJointDef weldJoitDef;
    weldJoitDef.Initialize(bulletBody, armBody, b2Vec2(450.0f/PTM_RATIO, (FLOOR_HEIGHT + 310.0f)/PTM_RATIO));
    weldJoitDef.collideConnected = false;   /* ����Ϊ����ײ�����ӣ��̳���b2JointDef */
    bulletJoint = (b2WeldJoint *)world->CreateJoint(&weldJoitDef);

    /* �����ڵ����� */
    return true;
}

void HelloWorld::createTarget(const char * imageName, CCPoint position, float rotation, bool isCircle, bool isStatic, bool isEnemy)
{
    /* �������� */
    CCSprite *sprite = CCSprite::create(imageName);
    this->addChild(sprite, 1);

    /* ����body */
    b2BodyDef bodyDef;
    /* sprite��getContentSize�̳���ccNode */
    bodyDef.position = b2Vec2((position.x + sprite->getContentSize().width /2.0f)/PTM_RATIO, 
                                            (position.y + sprite->getContentSize().height /2.0f)/PTM_RATIO);
    if (isStatic)
    {
        bodyDef.type = b2_staticBody;
    }
    else
    {   
         bodyDef.type = b2_dynamicBody;
    }
    bodyDef.angle = CC_DEGREES_TO_RADIANS(rotation);
    bodyDef.userData = (void *)sprite;
    b2Body * body = world->CreateBody(&bodyDef);

    b2FixtureDef boxDef;
    boxDef.density = 0.5f;

    if (isEnemy)
    {
        boxDef.userData = (void *)1;    /* ָ���ַΪ1���ڴ�᲻���Σ�� */
        enemies.push_back(body);
    }
    
    if (isCircle)
    {
        b2CircleShape  circle;
        circle.m_radius = sprite->getContentSize().width /2.0f/PTM_RATIO;
        boxDef.shape = &circle;
        body->CreateFixture(&boxDef);
    }
    else
    {
        b2PolygonShape box;
        box.SetAsBox(sprite->getContentSize().width /2.0f/PTM_RATIO,
                             sprite->getContentSize().height/2.0f/PTM_RATIO);
        boxDef.shape = &box;
        body->CreateFixture(&boxDef);
    }

    targets.push_back(body);

    return;
}

void HelloWorld::createTarges()
{
    createTarget("brick_2-hd.png", CCPointMake(675.0 * 2, FLOOR_HEIGHT), 0.0f, false, false, false); 
    createTarget("brick_1-hd.png", CCPointMake(741.0 * 2, FLOOR_HEIGHT), 0.0f, false, false, false); 
    createTarget("brick_1-hd.png", CCPointMake(741.0 * 2, FLOOR_HEIGHT+45.0), 0.0f, false, false, false); 
    createTarget("brick_3-hd.png", CCPointMake(673.0 * 2, FLOOR_HEIGHT+92.0), 0.0f, false, false, false); 
    createTarget("brick_1-hd.png", CCPointMake(707.0 * 2, FLOOR_HEIGHT+115.0), 0.0f, false, false, false); 
    createTarget("brick_1-hd.png", CCPointMake(707.0 * 2, FLOOR_HEIGHT+160.0), 0.0f, false, false, false); 
 
    createTarget("head_dog-hd.png", CCPointMake(702.0 * 2, FLOOR_HEIGHT), 0.0f, true, false, true); 
    createTarget("head_cat-hd.png", CCPointMake(680.0 * 2, FLOOR_HEIGHT+115.0), 0.0f, true, false, true); 
    createTarget("head_dog-hd.png", CCPointMake(740.0 * 2, FLOOR_HEIGHT+115.0), 0.0f, true, false, true); 
 
    // 2 bricks at the right of the first block 
    createTarget("brick_2-hd.png", CCPointMake(770.0 * 2, FLOOR_HEIGHT), 0.0f, false, false, false); 
    createTarget("brick_2-hd.png", CCPointMake(770.0 * 2, FLOOR_HEIGHT+92.0), 0.0f, false, false, false); 
 
    // The dog between the blocks 
    createTarget("head_dog-hd.png", CCPointMake(830.0 * 2, FLOOR_HEIGHT), 0.0f, true, false, true); 
 
    // Second block 
    createTarget("brick_platform-hd.png", CCPointMake(839.0 * 2, FLOOR_HEIGHT), 0.0f, false, true, false); 
    createTarget("brick_2-hd.png", CCPointMake(854.0 * 2, FLOOR_HEIGHT+28.0), 0.0f, false, false, false); 
    createTarget("brick_2-hd.png", CCPointMake(854.0 * 2, FLOOR_HEIGHT+28.0+92.0), 0.0f, false, false, false); 
    createTarget("head_cat-hd.png", CCPointMake(8160.0 * 2, FLOOR_HEIGHT+28.0), 0.0f, true, false, true); 
    createTarget("brick_2-hd.png", CCPointMake(909.0 * 2, FLOOR_HEIGHT+28.0), 0.0f, false, false, false); 
    createTarget("brick_1-hd.png", CCPointMake(909.0 * 2, FLOOR_HEIGHT+28.0+92.0), 0.0f, false, false, false); 
    createTarget("brick_1-hd.png", CCPointMake(909.0 * 2, FLOOR_HEIGHT+28.0+92.0+45.0), 0.0f, false, false, false); 
    createTarget("brick_2-hd.png", CCPointMake(882.0 * 2, FLOOR_HEIGHT+108.0), 90.0f, false, false, false);  
    return;
}

void HelloWorld::resetBullet()
{
    /* �ж��Ƿ��е��� */
    if (0 == enemies.size())
    {
        /* ������Ϸ */
        CCFiniteTimeAction *action = CCSequence::create(
                CCDelayTime::create(2.0f),
                CCCallFunc::create(this, callfunc_selector(HelloWorld::resetGame)),
                NULL);
        this->runAction(action);
    }
    else
    {
        /* װ���ڵ� */
        if (true == attachBullet())
        {
            /* �лػ�������� */
            CCAction * action = CCMoveTo::create(0.2f, CCPointZero);
            this->runAction(action);
        }
        else
        {
            /* ������Ϸ */
            CCFiniteTimeAction *action = CCSequence::create(
                    CCDelayTime::create(2.0f),
                    CCCallFunc::create(this, callfunc_selector(HelloWorld::resetGame)),
                    NULL);
            this->runAction(action);
        }
    }
}

void HelloWorld::resetGame()
{
    /* ����в����ڵ������ */
    if (bullets.size() != 0)
    {
        for (vector<b2Body *>::iterator iter = bullets.begin();
        iter != bullets.end();
        ++iter)
        {
            b2Body * body = *iter;
            this->removeChild((CCSprite *)body->GetUserData());
            world->DestroyBody(body);
        }   
        bullets.clear();
    }

    /* ����в�����˻�ש��������� */
    if (targets.size() != 0)
    {
        for (vector<b2Body *>::iterator iter = targets.begin();
        iter != targets.end();
        ++iter)
        {
            b2Body * body = *iter;
            this->removeChild((CCSprite *)body->GetUserData());
            world->DestroyBody(body);
        }   
        targets.clear();
        enemies.clear();
    }
    
    /* �����ɹ��ڵ� */
    createBullets(4);
    /* �����������Ŀ�� */
    createTarges();

    /* ���ƶ�����Ļ�ұ�(ʵ������layer����)��Ȼ���ڵ��뵯�ۺ��ӣ�ͣ1s��
   * �����˼����Ȼ�����»ص���Ļ���(ʵ������layer������) */
   CCFiniteTimeAction *action = CCSequence::create(
            CCMoveTo::create(1.5f, CCPointMake(-960, 0)),
            CCCallFunc::create(this, callfunc_selector(HelloWorld::attachBullet)),
            CCDelayTime::create(1.0f),
            CCMoveTo::create(1.5f, CCPointZero),
            NULL);
    this->runAction(action);
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    /* ɾ������ */
    CC_SAFE_DELETE(world);
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
