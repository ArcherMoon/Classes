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

    /* 弹臂处于未释放状态 */
    releasingArm = false;

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
    world->SetContinuousPhysics(true);

    /* 创造地面物体，
   * 定义物体(BodyDef)，由world根据BodyDef创造body，创造形状(shape)，
   * 定义夹具(FixtureDef)，指向shape, 由body根据FixtureDef创造Fixture */
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

    armBody = world->CreateBody(&armBodyDef);

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
    armJointDef.maxMotorTorque = 1500;                                       /* 最大扭矩，马达转动是需要扭动力的，越大则反弹力越大，抛出时的速度越大 */
    armJointDef.motorSpeed = -10;                                               /* 马达速度,1s内转多少度，弧度制 */
    armJoint = (b2RevoluteJoint *)world->CreateJoint(&armJointDef);   

    /* 初始化鼠标关节指针为空，使能触摸 */
    mouseJoint = NULL;
    this->setTouchEnabled(true);

    /* 重置游戏,预想是在弹臂右偏9度的方向绑定，但游戏刚开始时，
   * 弹臂还处在与Y轴0度的位置，需要修正一下，等一会，等到弹臂到偏9度的位置再绑定 */
   CCFiniteTimeAction *action = CCSequence::create(
            CCDelayTime::create(0.2f),
            CCCallFunc::create(this, callfunc_selector(HelloWorld::resetGame)),
            NULL);
    this->runAction(action);

    /* 创建碰撞监听器，并由世界监听 */
    contactListener = new MyContactListener();  /* 注意销毁 */
    world->SetContactListener(contactListener);
    
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

    /* 如果弹臂处于释放状态且小于一定角度，则删除焊接关节 */
    if (releasingArm && NULL != bulletJoint)
    {
            if (armJoint->GetJointAngle() <= CC_DEGREES_TO_RADIANS(30))
            {
                releasingArm =false;
                world->DestroyJoint(bulletJoint);
                bulletJoint = NULL;

                /* 炮弹射出几秒后，将画面移回最左边并重新装载炮弹 */
                /* layer先执行延时动作，再执行装载炮弹动作 */
                CCFiniteTimeAction * action = CCSequence::create(
                        CCDelayTime::create(5.0f),
                        CCCallFunc::create(this, callfunc_selector(HelloWorld::resetBullet)),
                        NULL);
                this->runAction(action);
            }
    }

    /* 判断炮弹是否在屏幕中心点的右边，移动视角 */
    if (NULL != bulletBody && NULL == bulletJoint)
    {
        /* 取得炮弹中心点 */
        b2Vec2 position = bulletBody->GetPosition();
        /* 取得layer的坐标,从ccNode继承getPosition */
        CCPoint myPosition = this->getPosition();
        /* 取得屏幕宽度 */
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

        if (position.x > screenSize.width /2.0f /PTM_RATIO)
        {
             /* 设置layer的新坐标,从ccNode继承setPosition */
             myPosition.x = -MIN(screenSize.width * 2.0f - screenSize.width, position.x * PTM_RATIO - screenSize.width /2.0f);
             this->setPosition(myPosition);
        }   
    }

    /* --销毁碰撞的敌人-- */
    /* 创建迭代器，遍历contacts销毁 */
    std::set<b2Body *>::iterator pos;
    for (pos = contactListener->contacts.begin();
           pos != contactListener->contacts.end();
           ++pos)
    {
        /* 取出指针指向的内容，即b2Body* */
        b2Body *body = *pos;

        /* 从targes中删除 */
        for (vector<b2Body *>::iterator iter = targets.begin();
                iter != targets.end();
                ++iter)
        {
            if (body == *iter)
            {
                /* 保存删除元素的下一个元素指针 */
                iter = targets.erase(iter);
                break;
            }
        }   

        /* 从enemies中删除 */
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

        /* 删除layer中对应的精灵 */
        CCNode *contactNode = (CCNode *)body->GetUserData();
        CCPoint position = contactNode->getPosition();
        this->removeChild(contactNode);

        /* 删除world中的body */
        world->DestroyBody(body);

        /* --增加粒子效果-- */
        CCParticleSun *explosion = CCParticleSun::create();
        /* 增加纹理 */
        explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire.png"));
        /* 一些参数 */
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
    /* 清空contacts，用于保存下次碰撞后要摧毁的敌人 */
    contactListener->contacts.clear();
    
}

void HelloWorld::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    /* 如果已经有鼠标关节说明正在移动中，返回 */
    if (NULL != mouseJoint)
    {
        return;
    }
    
    /* 取得任一touch */
    CCTouch *touch = (CCTouch *)pTouches->anyObject();
    /* 获取坐标 */
    CCPoint location = touch->getLocation();
    /* 转化为box2d坐标 */
    b2Vec2 target = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);

    /* 创建鼠标关节 */
    if (target.x < armBody->GetWorldCenter().x + 50.0f/PTM_RATIO)
    {
        b2MouseJointDef mouceJointDef;
        /* 这里的bodyA,bodyB是结构体b2MouseJointDef从b2JointDef继承而来 */
        mouceJointDef.bodyA = groundBody;
        mouceJointDef.bodyB = armBody;
        mouceJointDef.target = target;
        mouceJointDef.maxForce = 2000;  /* 过小则不能拉动弹臂，不影响弹臂的反弹力 */
        mouseJoint = (b2MouseJoint *)world->CreateJoint(&mouceJointDef);
    }
}

void HelloWorld::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    /* 如果没有鼠标关节直接返回 */
    if (NULL == mouseJoint)
    {
        return;
    }
    
    /* 取得任一touch */
    CCTouch *touch = (CCTouch *)pTouches->anyObject();
    /* 获取坐标 */
    CCPoint location = touch->getLocation();
    /* 转化为box2d坐标 */
    b2Vec2 target = b2Vec2(location.x/PTM_RATIO, location.y/PTM_RATIO);

    /* 修改鼠标关节target的值 */
    mouseJoint->SetTarget(target);
    return;
}

void HelloWorld::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
    /* 销毁鼠标关节 */
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
    /* 当前松果在数组中的序号 */
    currentBullet = 0;
    /* 第一个松果的摆放位置，注意为物体的中心点 */
    float pos = 140.0f;
    float delta = 0.0f;

    /*  计算从左边松鼠+半个松果到炮台-半个松果之间摆放所有松果的间距 */
    if (count > 1)
    {
        delta = (320.0f - 30.0f - pos) / (count - 1);
    }

    /* 循环创建松果body，加入数组 */
    CCSprite *sprite = NULL;
    b2Body * bullet = NULL;
    for (int i=0; i < count; i++, pos += delta)
    {
        sprite = CCSprite::create("acorn-hd.png");
        this->addChild(sprite, 1);

        b2BodyDef bulletBodyDef;
        bulletBodyDef.type = b2_dynamicBody;
        bulletBodyDef.bullet = true;  /* 高速物体会做持续碰撞检测(CCD)处理 */
        bulletBodyDef.position.Set(pos/PTM_RATIO, (FLOOR_HEIGHT + 28.0f)/PTM_RATIO);
        bulletBodyDef.userData = (void *)sprite;
        bullet = world->CreateBody(&bulletBodyDef);
        bullet->SetActive(false);       /* 虽然是动态物体，先不激活，提高效率 */

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
    /* 如果当前炮弹的序号大于等于总数，则返回 */
    if (currentBullet >= bullets.size())
    {
        return false;
    }
    
    /* 由当前炮弹序号找到当前炮弹 */
    bulletBody = bullets.at(currentBullet++);
    /* 移动到弹臂处 */
    bulletBody->SetTransform(b2Vec2(450.0f/PTM_RATIO, (FLOOR_HEIGHT + 310.0f)/PTM_RATIO), 0.0f);
    /* 激活，否则后面焊接没有效果 */
    bulletBody->SetActive(true);

    /* 创建焊接关节，将炮弹与弹臂焊接在一起 */
    b2WeldJointDef weldJoitDef;
    weldJoitDef.Initialize(bulletBody, armBody, b2Vec2(450.0f/PTM_RATIO, (FLOOR_HEIGHT + 310.0f)/PTM_RATIO));
    weldJoitDef.collideConnected = false;   /* 设置为不碰撞的连接，继承自b2JointDef */
    bulletJoint = (b2WeldJoint *)world->CreateJoint(&weldJoitDef);

    /* 还有炮弹可用 */
    return true;
}

void HelloWorld::createTarget(const char * imageName, CCPoint position, float rotation, bool isCircle, bool isStatic, bool isEnemy)
{
    /* 创建精灵 */
    CCSprite *sprite = CCSprite::create(imageName);
    this->addChild(sprite, 1);

    /* 创建body */
    b2BodyDef bodyDef;
    /* sprite的getContentSize继承自ccNode */
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
        boxDef.userData = (void *)1;    /* 指向地址为1的内存会不会很危险 */
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
    /* 判断是否还有敌人 */
    if (0 == enemies.size())
    {
        /* 重置游戏 */
        CCFiniteTimeAction *action = CCSequence::create(
                CCDelayTime::create(2.0f),
                CCCallFunc::create(this, callfunc_selector(HelloWorld::resetGame)),
                NULL);
        this->runAction(action);
    }
    else
    {
        /* 装载炮弹 */
        if (true == attachBullet())
        {
            /* 切回画面最左边 */
            CCAction * action = CCMoveTo::create(0.2f, CCPointZero);
            this->runAction(action);
        }
        else
        {
            /* 重置游戏 */
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
    /* 如果有残余炮弹先清除 */
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

    /* 如果有残余敌人或砖块先先清除 */
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
    
    /* 创建松果炮弹 */
    createBullets(4);
    /* 批量创建打击目标 */
    createTarges();

    /* 先移动到屏幕右边(实际是让layer左移)，然后将炮弹与弹臂焊接，停1s，
   * 让玩家思考，然后重新回到屏幕左边(实际是让layer再右移) */
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
    /* 删除世界 */
    CC_SAFE_DELETE(world);
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
