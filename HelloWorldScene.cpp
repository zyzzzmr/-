/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 *****************************************************************************/

//2150228 朱昀哲

#include "HelloWorldScene.h"
#include "ui/CocosGUI.h"
#include "audio/include/AudioEngine.h"
using namespace cocos2d::ui;
using namespace std;
/* 标注一些无用参数
屏幕大小：1300*800（像素）
角色贴图大小：75*70（像素）
墙贴图大小：40*40（像素）
土地贴图大小：25*25（像素）
土地布局大小：83*27（个)
*/
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}



// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    // 界面
    const auto visibleSize = Size(1300, 800);
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 一些参数
    // 模式控制
    static bool effect = 1;//炮弹：1==爆炸弹，0==穿甲弹
    static bool life = 1;//生命：1==无限，0==有限
    static bool climb = 1;//攀爬：1==关，0==开
    static bool ai = 1;//对手：1==热座，0==人机
    static bool level = 1;//人机难度：1==简单，0==困难
    static bool geography = 1;//地图：1==联通，0==隔离
    //横纵坐标（地块格版）
    static int placely[2] = { 19,27 };
    static int placelm[2] = { 62,27 };

    static healthpoint hp;
    //音乐与音效控制
    static bool musicOn = false;
    static bool soundOn = false;
    //地图
    static int map[83][27];
    //帮助控制
    static int help = 1;
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    /////////////////////////////
    // 3. add your codes below...
    /***************************开始界面***************************/
    //标题
    // add "HelloWorld" splash screen"
    auto background = Sprite::create("hello.png");
    if (background == nullptr)
    {
        problemLoading("'hello.png'");
    }
    else
    {
        background->setScaleX(visibleSize.width / background->getContentSize().width);
        background->setScaleY(visibleSize.height / background->getContentSize().height);
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(background, 0);
    }

    //创建音乐按钮
    int bgmID = -1;
    auto musicButton = Button::create("MusicOff.png", "MusicOn.png");
    musicButton->setPosition(Vec2(visibleSize.width - 100, visibleSize.height - 50));
    musicButton->addClickEventListener([=](Ref* sender) mutable {
        musicOn = !musicOn;
        if (musicOn) 
        {
            musicButton->loadTextures("MusicOn.png", "MusicOff.png");
            //播放背景音乐（三国志曹操传：：Se_e_07【过场动画-战乱】）
            if (bgmID == -1) 
            {
                bgmID = AudioEngine::play2d("bgm.mp3", true);
            }
            else 
            {
                AudioEngine::resume(bgmID);
            }
        }
        else 
        {
            musicButton->loadTextures("MusicOff.png", "MusicOn.png");
            //暂停背景音乐
            if (bgmID != -1) 
            {
                AudioEngine::pause(bgmID);
            }
        }
        });
    this->addChild(musicButton);

    //创建音效按钮
    auto soundButton = Button::create("SoundOff.png", "SoundOn.png");
    soundButton->setPosition(Vec2(visibleSize.width - 50, visibleSize.height - 50));
    soundButton->addClickEventListener([=](Ref* sender) mutable {
        soundOn = !soundOn;
        if (soundOn) 
        {
            soundButton->loadTextures("SoundOn.png", "SoundOff.png");
            //启用音效
        }
        else 
        {
            soundButton->loadTextures("SoundOff.png", "SoundOn.png");
            //关闭音效
        }
        });

    this->addChild(soundButton);
    //创建帮助按钮
    auto helpButton = Button::create("helper0.png", "starter.png");
    if (helpButton == nullptr)
    {
        problemLoading("'helper0.png', or 'starter.png'");
    }
    else
    {
        helpButton->setPosition(Vec2(visibleSize.width - 100, visibleSize.height - 100));
        this->addChild(helpButton, 3);
    }
    //创建帮助图例
    bool isHelperVisible = false; //标志helper是否可见
    Node* helperContainer = nullptr; //用于包装helper的容器节点
    helpButton->addClickEventListener([=](Ref* sender) mutable {
        if (!isHelperVisible) 
        {
            //如果helper不可见，则创建并添加它到容器节点
            if (help)//显示开始界面帮助
            {
                auto helper = Sprite::create("helper2.png");
                if (helper == nullptr) 
                {
                    problemLoading("'helper2.png'");
                }
                else 
                {
                    if (helperContainer == nullptr) 
                    {
                        helperContainer = Node::create();
                        helperContainer->setScaleX(visibleSize.width / helper->getContentSize().width);
                        helperContainer->setScaleY(visibleSize.height / helper->getContentSize().height);
                        helperContainer->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
                        this->addChild(helperContainer, 2);
                    }
                    helperContainer->addChild(helper);
                    isHelperVisible = true; 
                }
            }
            else//显示游戏界面帮助
            {
                auto helper = Sprite::create("helper.png");
                if (helper == nullptr) 
                {
                    problemLoading("'helper.png'");
                }
                else 
                {
                    if (helperContainer == nullptr) 
                    {
                        helperContainer = Node::create();
                        helperContainer->setScaleX(visibleSize.width / helper->getContentSize().width);
                        helperContainer->setScaleY(visibleSize.height / helper->getContentSize().height);
                        helperContainer->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
                        this->addChild(helperContainer, 2);
                    }
                    helperContainer->addChild(helper);
                    isHelperVisible = true; 
                }
            }
        }
        else 
        {
            //如果helper已可见，则从容器节点中移除它
            if (helperContainer != nullptr && helperContainer->getChildrenCount() > 0) 
            {
                helperContainer->removeAllChildren();
            }
            isHelperVisible = false;
        }
        });

    /***************************菜单*********************************/
    //炮弹选择
    auto effectButton = Button::create("effect1.png", "effect2.png");
    effectButton->setPosition(Vec2(50, visibleSize.height - 50));
    effectButton->addClickEventListener([=](Ref* sender) mutable {
        effect = !effect;
        if (effect) 
        {
            effectButton->loadTextures("effect1.png", "effect2.png");
        }
        else 
        {
            effectButton->loadTextures("effect2.png", "effect1.png");
        }
        });
    this->addChild(effectButton);
    //生命选择
    auto lifeButton = Button::create("life1.png", "life2.png");
    lifeButton->setPosition(Vec2(50, visibleSize.height - 100));
    lifeButton->addClickEventListener([=](Ref* sender) mutable {
        life = !life;
        if (life) 
        {
            lifeButton->loadTextures("life1.png", "life2.png");
        }
        else 
        {
            lifeButton->loadTextures("life2.png", "life1.png");
        }
        });
    this->addChild(lifeButton);
    //技能选择
    auto climbButton = Button::create("climb1.png", "climb2.png");
    climbButton->setPosition(Vec2(50, visibleSize.height - 150));
    climbButton->addClickEventListener([=](Ref* sender) mutable {
        climb = !climb;
        if (climb) 
        {
            climbButton->loadTextures("climb1.png", "climb2.png");
        }
        else 
        {
            climbButton->loadTextures("climb2.png", "climb1.png");
        }
        });
    this->addChild(climbButton);
    //对手选择
    auto aiButton = Button::create("ai1.png", "ai2.png");
    aiButton->setPosition(Vec2(50, visibleSize.height - 200));
    aiButton->addClickEventListener([=](Ref* sender) mutable {
        ai = !ai;
        if (ai) 
        {
            aiButton->loadTextures("ai1.png", "ai2.png");
        }
        else 
        {
            aiButton->loadTextures("ai2.png", "ai1.png");
        }
        });
    this->addChild(aiButton);
    //人机难度选择
    auto levelButton = Button::create("level1.png", "level2.png");
    levelButton->setPosition(Vec2(50, visibleSize.height - 250));
    levelButton->addClickEventListener([=](Ref* sender) mutable {
        level = !level;
        if (level) 
        {
            levelButton->loadTextures("level1.png", "level2.png");
        }
        else 
        {
            levelButton->loadTextures("level2.png", "level1.png");
        }
        });
    this->addChild(levelButton);
    //地图选择
    auto geographyButton = Button::create("geography1.png", "geography2.png");
    geographyButton->setPosition(Vec2(50, visibleSize.height - 300));
    geographyButton->addClickEventListener([=](Ref* sender) mutable {
        geography = !geography;
        if (geography) 
        {
            geographyButton->loadTextures("geography1.png", "geography2.png");
        }
        else 
        {
            geographyButton->loadTextures("geography2.png", "geography1.png");
        }
        });
    this->addChild(geographyButton);

    auto winner = Sprite::create("winner.png");
    //创建开始按钮精灵（三国志曹操传：：金火罐炮）
    auto startButton = Button::create("starter0.png", "starter.png", "0.png");
    if (startButton == nullptr)
    {
        problemLoading("'starter0.png', or'starter.png'");
    }
    else
    {
        startButton->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        this->addChild(startButton, 3);

        //注册监听器
        startButton->addClickEventListener([=](Ref* sender) mutable {

            //删除开始界面
            this->removeChild(startButton, true);
            this->removeChild(background, true);
            this->removeChild(effectButton, true);
            this->removeChild(lifeButton, true);
            this->removeChild(climbButton, true);
            this->removeChild(aiButton, true);
            this->removeChild(levelButton, true);
            this->removeChild(geographyButton, true);

            /*=======================初始化===============================*/
            //更换帮助界面
            help = 0;
            //游戏标题
            auto label = Label::createWithTTF("the War of Catapults", "fonts/simsunb.ttf", 32);
            if (label == nullptr)
            {
                problemLoading("'fonts/Marker Felt.ttf'");
            }
            else
            {
                label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 3 / 4 + 175));
                this->addChild(label, 1);
            }
            //红方角色（三国志曹操传：：刘晔）//ly=>liuye
            auto spritely = Sprite::create("liuye.png");
            if (spritely == nullptr)
            {
                problemLoading("'liuye.png'");
            }
            else
            {
                spritely->setPosition(Vec2(visibleSize.width / 4 + origin.x, 445));
                this->addChild(spritely, 0);
            }
            //蓝方角色（三国志姜维传：：刘敏）//lm=>liumin
            auto spritelm = Sprite::create("liumin.png");
            if (spritelm == nullptr)
            {
                problemLoading("'liumin.png'");
            }
            else
            {
                spritelm->setPosition(Vec2(visibleSize.width / 4 * 3 + origin.x, 445));
                this->addChild(spritelm, 0);
            }

            //墙左上（三国志曹操传：：城墙）//u.d.l.r=>up.down.left.right
            Sprite* wallul = Sprite::create("wallul.png");
            if (wallul == nullptr)
            {
                problemLoading("'wallul.png'");
            }
            else
            {
                wallul->setPosition(10, visibleSize.height - 10);
                this->addChild(wallul, 0);
            }

            //墙右上（三国志曹操传：：城墙）
            Sprite* wallur = Sprite::create("wallur.png");
            if (wallur == nullptr)
            {
                problemLoading("'wallur.png'");
            }
            else
            {
                wallur->setPosition(visibleSize.width - 10, visibleSize.height - 10);
                this->addChild(wallur, 0);
            }

            //墙左下（三国志曹操传：：城墙）
            Sprite* walldl = Sprite::create("walldl.png");
            if (walldl == nullptr)
            {
                problemLoading("'walldl.png'");
            }
            else
            {
                walldl->setPosition(10, 10);
                this->addChild(walldl, 0);
            }

            //墙右下（三国志曹操传：：城墙）
            Sprite* walldr = Sprite::create("walldr.png");
            if (walldr == nullptr)
            {
                problemLoading("'walldr.png'");
            }
            else
            {
                walldr->setPosition(visibleSize.width - 10, 10);
                this->addChild(walldr, 0);
            }

            //墙上（三国志曹操传：：城墙）
            Sprite* wallu[63];
            for (int i = 0; i < 63; i++)
            {
                wallu[i] = Sprite::create("wallud.png");
                if (wallu[i] == nullptr)
                {
                    problemLoading("'wallud.png'");
                }
                else
                {
                    wallu[i]->setPosition(static_cast<float>(30 + i * 20), visibleSize.height - 10);
                    this->addChild(wallu[i], 0);
                }
            }

            //墙下（三国志曹操传：：城墙）
            Sprite* walld[63];
            for (int i = 0; i < 63; i++)
            {
                walld[i] = Sprite::create("wallud.png");
                if (walld[i] == nullptr)
                {
                    problemLoading("'wallud.png'");
                }
                else
                {
                    walld[i]->setPosition(static_cast<float>(30 + i * 20), static_cast<float>(10));
                    this->addChild(walld[i], 0);
                }
            }

            //墙左（三国志曹操传：：城墙）
            Sprite* walll[38];
            for (int i = 0; i < 38; i++)
            {
                walll[i] = Sprite::create("walllr.png");
                if (walll[i] == nullptr)
                {
                    problemLoading("'walllr.png'");
                }
                else
                {
                    walll[i]->setPosition(static_cast<float>(10), static_cast<float>(30 + i * 20));
                    this->addChild(walll[i], 0);
                }
            }

            //墙右（三国志曹操传：：城墙）
            Sprite* wallr[38];
            for (int i = 0; i < 38; i++)
            {
                wallr[i] = Sprite::create("walllr.png");
                if (wallr[i] == nullptr)
                {
                    problemLoading("'walllr.png'");
                }
                else
                {
                    wallr[i]->setPosition(visibleSize.width - 10, static_cast<float>(30 + i * 20));
                    this->addChild(wallr[i], 0);
                }
            }

            //土地
            Sprite* ground[83][27];
            for (int i = 0; i < 83; i++)
            {
                for (int j = 0; j < 27; j++)
                {
                    map[i][j] = 1;
                    ground[i][j] = Sprite::create("ground.png");
                    if (ground[i][j] == nullptr)
                    {
                        problemLoading("'ground.png'");
                    }
                    else
                    {
                        ground[i][j]->setPosition(static_cast<float>(35 + i * 15), static_cast<float>(30 + j * 15));
                        this->addChild(ground[i][j], 0);
                    }
                }
            }
            if (!geography)
            {
                for (int i = 40; i < 43; i++)
                {
                    for (int j = 0; j < 27; j++)
                    {
                        map[i][j] = 0;
                        this->removeChild(ground[i][j], true);
                    }
                }
            }
            /*****************************游戏*********************************/
            //当前回合角色
            int firer = 1;
            //strength可视化
            Label* strengthLabell;
            strengthLabell = Label::createWithTTF("LeftStrength: 10", "fonts/arial.ttf", 24);
            strengthLabell->setPosition(Vec2(100, visibleSize.height - 150));
            this->addChild(strengthLabell, 1);
            Label* strengthLabelr;
            strengthLabelr = Label::createWithTTF("RightStrength: 10", "fonts/arial.ttf", 24);
            strengthLabelr->setPosition(Vec2(100, visibleSize.height - 175));
            this->addChild(strengthLabelr, 1);
            //hp可视化
            Label* hplyLabel;
            Label* hplmLabel;
            if (!life)
            {
                hplyLabel = Label::createWithTTF("Red's HP: 100", "fonts/arial.ttf", 24);
                hplyLabel->setPosition(Vec2(100, visibleSize.height - 200));
                hplyLabel->setTextColor(Color4B::RED);
                this->addChild(hplyLabel, 1);
                hplmLabel = Label::createWithTTF("Blue's HP: 100", "fonts/arial.ttf", 24);
                hplmLabel->setPosition(Vec2(100, visibleSize.height - 225));
                hplmLabel->setTextColor(Color4B::BLUE);
                this->addChild(hplmLabel, 1);
            }
            //turn可视化
            Label* turnLabel;
            turnLabel = Label::createWithTTF("now red can fire!", "fonts/arial.ttf", 24);
            turnLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 150));
            this->addChild(turnLabel, 1);

            //创建力度滑块
            auto linearSlider = Slider::create();
            linearSlider->loadBarTexture("arrow0.png");
            linearSlider->loadSlidBallTextures("shell.png", "shell1.png");
            linearSlider->setPercent(50); // 设置初始值（0到100）
            linearSlider->setPosition(Vec2(300, 700));
            this->addChild(linearSlider);
            //创建角度滑块
            auto radialSlider = Slider::create();
            radialSlider->loadBarTexture("angleer.png");
            radialSlider->loadSlidBallTextures("shell.png", "shell1.png");
            radialSlider->setPercent(50); // 设置初始值（0到100）
            radialSlider->setPosition(Vec2(500, 700));
            this->addChild(radialSlider);

            //创建开火按钮
            auto recordButton = Button::create("shell.png", "shell1.png", "0.png");
            recordButton->setPosition(Vec2(400, 650));
            recordButton->addClickEventListener([=](Ref* sender) mutable {
                int strengthl = 10, strengthr = 10;

                //创建左移按钮
                auto moveLeftButton = Button::create("lefter.png", "lefter.png", "0.png");
                if (moveLeftButton == nullptr)
                {
                    problemLoading("'lefter.png' or 'lefter.png'");
                }
                else
                {
                    moveLeftButton->setPosition(Vec2(50, 700));
                    this->addChild(moveLeftButton, 1);
                    moveLeftButton->addClickEventListener([=](Ref* sender) mutable {
                        if (strengthl > 0)
                        {
                            if (ai)
                            {
                                if (firer == 1 && placely[0] != 0 && (placely[1] == 27 || map[placely[0] - 1][placely[1]] == 0))
                                {
                                    //精灵"spritely"左移25
                                    move(spritely, placely, 1, 1);
                                }
                                else if (firer == -1 && placelm[0] != 0 && (placelm[1] == 27 || map[placelm[0] - 1][placelm[1]] == 0))
                                {
                                    //精灵"spritelm"左移25
                                    move(spritelm, placelm, 1, 1);
                                }
                                else if (climb == 0 && firer == 1 && placely[0] != 0 && (placely[1] == 26 || map[placely[0] - 1][placely[1] + 1] == 0))
                                {
                                    //精灵"spritely"左上移25
                                    move(spritely, placely, 1, 0);
                                }
                                else if (climb == 0 && firer == -1 && placelm[0] != 0 && (placelm[1] == 26 || map[placelm[0] - 1][placelm[1] + 1] == 0))
                                {
                                    //精灵"spritelm"左上移25
                                    move(spritelm, placelm, 1, 0);
                                }
                            }
                            else
                            {
                                if (placely[0] != 0 && (placely[1] == 27 || map[placely[0] - 1][placely[1]] == 0))
                                {
                                    //精灵"spritely"左移25
                                    move(spritely, placely, 1, 1);
                                }
                                else if (climb == 0 && placely[0] != 0 && (placely[1] == 26 || map[placely[0] - 1][placely[1] + 1] == 0))
                                {
                                    //精灵"spritely"左上移25
                                    move(spritely, placely, 1, 0);
                                }
                            }
                            //strength减少
                            strengthl--;
                            //更新strengthLabel的文本内容
                            string strengthStrl = "LeftStrength: " + to_string(strengthl);
                            strengthLabell->setString(strengthStrl);
                            //进行角色重力检测
                            while (map[placely[0]][placely[1] - 1] == 0 && map[placely[0] + 1][placely[1] - 1] == 0)
                            {
                                spritely->setPositionY(spritely->getPositionY() - 15);
                                placely[1]--;
                            }
                            while (map[placelm[0]][placelm[1] - 1] == 0 && map[placelm[0] + 1][placelm[1] - 1] == 0)
                            {
                                spritelm->setPositionY(spritelm->getPositionY() - 15);
                                placelm[1]--;
                            }
                            //移动音效（三国志曹操传：：Se25【角色移动-炮车】）
                            AudioEngine::setVolume(AudioEngine::play2d("move.mp3"), (soundOn ? 1.0f : 0.0f));
                        }
                        });
                }
                //创建右移按钮
                auto moveRightButton = Button::create("righter.png", "righter.png", "0.png");
                if (moveRightButton == nullptr)
                {
                    problemLoading("'righter.png' or 'righter.png'");
                }
                else
                {
                    moveRightButton->setPosition(Vec2(150, 700));
                    this->addChild(moveRightButton, 1);
                    moveRightButton->addClickEventListener([=](Ref* sender) mutable {
                        if (strengthr > 0)
                        {
                            if (ai)
                            {
                                if (firer == 1 && placely[0] != 81 && (placely[1] == 27 || map[placely[0] + 2][placely[1]] == 0))
                                {
                                    //精灵"spritely"右移25
                                    move(spritely, placely, 0, 1);
                                }
                                else if (firer == -1 && placelm[0] != 81 && (placelm[1] == 27 || map[placelm[0] + 2][placelm[1]] == 0))
                                {
                                    //精灵"spritelm"右移25
                                    move(spritelm, placelm, 0, 1);
                                }
                                else if (climb == 0 && firer == 1 && placely[0] != 81 && (placely[1] == 26 || map[placely[0] + 2][placely[1] + 1] == 0))
                                {
                                    //精灵"spritely"右上移25
                                    move(spritely, placely, 0, 0);
                                }
                                else if (climb == 0 && firer == -1 && placelm[0] != 81 && (placelm[1] == 26 || map[placelm[0] + 2][placelm[1] + 1] == 0))
                                {
                                    //精灵"spritelm"右上移25
                                    move(spritelm, placelm, 0, 0);
                                }
                            }
                            else
                            {
                                if (placely[0] != 81 && (placely[1] == 27 || map[placely[0] + 2][placely[1]] == 0))
                                {
                                    //精灵"spritely"右移25
                                    move(spritely, placely, 0, 1);
                                }
                                else if (climb == 0 && placely[0] != 81 && (placely[1] == 26 || map[placely[0] + 2][placely[1] + 1] == 0))
                                {
                                    //精灵"spritely"右上移25
                                    move(spritely, placely, 0, 0);
                                }
                            }
                            //strength减少
                            strengthr--;
                            //更新strengthLabel的文本内容
                            string strengthStrr = "RightStrength: " + to_string(strengthr);
                            strengthLabelr->setString(strengthStrr);
                            //进行角色重力检测
                            check(map, placely, placelm, spritely, spritelm);
                            AudioEngine::setVolume(AudioEngine::play2d("move.mp3"), (soundOn ? 1.0f : 0.0f));
                        }
                        });
                }
                //创建结束回合按钮
                auto endButton = Button::create("ender.png", "starter.png", "0.png");
                endButton->setPosition(Vec2(visibleSize.width / 2, 700));
                endButton->addClickEventListener([=](Ref* sender) mutable {
                    //启用recordButton，禁用moveLeftButton和moveRightButton
                    recordButton->setEnabled(true);
                    moveLeftButton->setEnabled(false);
                    moveRightButton->setEnabled(false);
                    //更新turnLabel的文本内容
                    string turnStr;
                    if (ai)
                    {
                        if (firer == -1)
                        {
                            turnStr = "now red can fire!";
                        }
                        else
                        {
                            turnStr = "now blue can fire!";
                        }
                    }
                    else
                    {
                        turnStr = "now red can fire!";
                    }

                    turnLabel->setString(turnStr);
                    //扣减体力
                    if (!life)
                    {
                        //更新hpLabel的文本内容
                        string hplyStr = "Red's HP: " + to_string(hp.getHply());
                        hplyLabel->setString(hplyStr);
                        string hplmStr = "Blue's HP: " + to_string(hp.getHplm());
                        hplmLabel->setString(hplmStr);
                    }
                    //进行胜利检测
                    if (placely[1] <= 0 ||(!life && hp.getHply() <= 0))
                    {
                        //胜利音效（三国志曹操传：：Se13【角色-升级】）
                        AudioEngine::play2d("win.mp3");
                        //创建胜利界面（三国杀online：：背景图-守卫剑阁）
                        auto winner = Sprite::create("winnerlm.png");
                        if (winner == nullptr)
                        {
                            problemLoading("'winnerlm.png'");
                        }
                        else
                        {
                            winner->setScaleX(visibleSize.width / winner->getContentSize().width);
                            winner->setScaleY(visibleSize.height / winner->getContentSize().height);
                            winner->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
                            this->addChild(winner, 2);
                        }
                    }
                    else if (placelm[1] <= 0 || (!life && hp.getHplm() <= 0))
                    {
                        //胜利音效（三国志曹操传：：Se13【角色-升级】）
                        AudioEngine::play2d("win.mp3");
                        //创建胜利界面（三国杀online：：背景图-守卫剑阁）
                        auto winner = Sprite::create("winnerly.png");
                        if (winner == nullptr)
                        {
                            problemLoading("'winnerly.png'");
                        }
                        else
                        {
                            winner->setScaleX(visibleSize.width / winner->getContentSize().width);
                            winner->setScaleY(visibleSize.height / winner->getContentSize().height);
                            winner->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
                            this->addChild(winner, 2);
                        }
                    }
                    //对战ai
                    if (!ai)
                    {
                        firer = -firer;
                        float powerai, angleai;
                        if (level)
                        {
                            //简单电脑的开火方案：随机参数
                            srand((unsigned int)(time(0)));
                            powerai = static_cast<float>((rand() % (90 - 30)) + 30);
                            if (placely[0] < placelm[0])
                            {
                                angleai = static_cast<float>((rand() % (150 - 100)) + 100);
                            }
                            else
                            {
                                angleai = static_cast<float>((rand() % (80 - 30)) + 30);
                            }
                        }
                        else
                        {
                            //困难电脑的开火方案：炮角固定60°，力度解方程得出
                            if (placely[0] < placelm[0])
                            {
                                angleai = 120;
                            }
                            else
                            {
                                angleai = 60;
                            }
                            powerai = static_cast<float>(fabs(spritely->getPosition().x - spritelm->getPosition().x) / (6 * sqrt((sqrt(3) * fabs(spritely->getPosition().x - spritelm->getPosition().x) - spritely->getPosition().y + spritelm->getPosition().y) / 490)));
                            //特别注释：二维匀加速直线运动方程联立求解t与powerai表达式
                        }
                        auto fire = Sprite::create("shell.png");
                        fire->setPosition(spritelm->getPosition());
                        this->addChild(fire, 0);

                        //开火音效（三国志曹操传：：Se_m_01【策略-烈火】）
                        AudioEngine::setVolume(AudioEngine::play2d("fire.mp3"), (soundOn ? 1.0f : 0.0f));

                        float gravity = -980; //重力加速度
                        //计算炮弹的水平速度和垂直速度
                        float initialVelocityX = 12 * powerai * cos(CC_DEGREES_TO_RADIANS(angleai));
                        float initialVelocityY = 12 * powerai * sin(CC_DEGREES_TO_RADIANS(angleai));

                        //创建动作，模拟炮弹的抛物线运动

                        auto action = Sequence::create(
                            MoveBy::create(1.0f, Vec2(initialVelocityX, initialVelocityY)),
                            CallFunc::create([=]() {
                                //炮弹运动结束后的回调函数
                                this->removeChild(fire, true);
                                }),
                            nullptr
                        );
                        //运行动作，并在每一帧更新垂直速度
                        fire->runAction(RepeatForever::create(Sequence::create(
                            DelayTime::create(static_cast<float>(0.016)), //帧率为 60 帧/秒，每帧的时间间隔为 1/60 秒
                            CallFunc::create([=]() mutable {
                                initialVelocityY += gravity * static_cast<float>(0.016); //根据重力更新垂直速度
                                Vec2 newPosition = fire->getPosition() + Vec2(initialVelocityX * static_cast<float>(0.016), initialVelocityY * static_cast<float>(0.016)); // 更新炮弹位置
                                //进行越界检测
                                if (newPosition.x + fire->getContentSize().width<0 || newPosition.x - fire->getContentSize().width >visibleSize.width || newPosition.y + fire->getContentSize().height < 0)
                                {
                                    //删除炮弹
                                    this->removeChild(fire, true);
                                    //可以结束阶段
                                    endButton->setEnabled(true);
                                }
                                //计算炮弹的边界框
                                Rect fireBoundingBox = Rect(newPosition.x, newPosition.y, fire->getContentSize().width, fire->getContentSize().height);

                                //进行碰撞检测
                                for (int i = 0; i < 83; i++) 
                                {
                                    for (int j = 0; j < 27; j++) 
                                    {
                                        if (map[i][j] == 1) //只检查地面精灵
                                        { 
                                            Rect groundBoundingBox = Rect(static_cast<float>(35 + i * 15), static_cast<float>(30 + j * 15), ground[i][j]->getContentSize().width, ground[i][j]->getContentSize().height);
                                            //shell与ground[i][j]碰撞
                                            if (fireBoundingBox.intersectsRect(groundBoundingBox)) 
                                            {
                                                //碰撞音效（三国志曹操传：：Se_m_07【策略-山岚】）
                                                AudioEngine::setVolume(AudioEngine::play2d("crash.mp3"), (soundOn ? 1.0f : 0.0f));
                                                //计算扣减生命
                                                if (!life)
                                                {
                                                    relife(i, j, placely, placelm, effect, &hp);
                                                }
                                                if (effect)
                                                {
                                                    //删除 shell 和 ground[i][j]
                                                    this->removeChild(fire, true);
                                                    this->removeChild(ground[i][j], true);
                                                    //可以结束阶段
                                                    endButton->setEnabled(true);
                                                    //删除周围的地面精灵
                                                    for (int dx = -3; dx <= 3; dx++) {
                                                        for (int dy = -2; dy <= 2; dy++) {
                                                            if (abs(dx) + abs(dy) > 3)
                                                            {
                                                                continue;
                                                            }
                                                            int nx = i + dx;
                                                            int ny = j + dy;
                                                            if (nx >= 0 && nx < 83 && ny >= 0 && ny < 27 && map[nx][ny] == 1)
                                                            {
                                                                this->removeChild(ground[nx][ny], true);
                                                                map[nx][ny] = 0; //标记为已删除
                                                                //进行角色重力检测
                                                                check(map, placely, placelm, spritely, spritelm);
                                                            }
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    //删除ground[i][j]
                                                    this->removeChild(ground[i][j], true);
                                                    //可以结束阶段
                                                    endButton->setEnabled(true);
                                                    //删除周围的地面精灵
                                                    map[i][j] = 0; // 标记为已删除
                                                    //进行角色重力检测
                                                    check(map, placely, placelm, spritely, spritelm);
                                                }
                                            }
                                        }
                                    }
                                }

                                fire->setPosition(newPosition); //更新炮弹位置
                                }),
                            nullptr
                        )));
                        int moveai = (rand() % 21);
                        for (int i = 0; i < 10; i++)
                        {
                            //对于困难ai来说，鉴于其固定了发射角度，尽量往右边跑是比较合理的；对于简单ai而言，左右次序无所谓；总之，先右再左
                            if (placelm[0] != 81 && (placelm[1] == 27 || map[placelm[0] + 2][placelm[1]] == 0))
                            {
                                //困难ai的加强方案：悬崖勒马
                                if (!level && map[placelm[0] + 2][placelm[1] - 1] == 0)
                                {
                                    break;
                                }
                                //简单电脑的移动方案：随机移动
                                move(spritelm, placelm, 0, 1);
                                //移动音效（三国志曹操传：：Se25【角色移动-炮车】）
                                AudioEngine::setVolume(AudioEngine::play2d("move.mp3"), (soundOn ? 1.0f : 0.0f));
                            }
                            else if (climb == 0 && placelm[0] != 81 && (placelm[1] == 26 || map[placelm[0] + 2][placelm[1] + 1] == 0))
                            {
                                //不管简单困难，能往上爬总是好的
                                move(spritelm, placelm, 0, 0);
                                //移动音效（三国志曹操传：：Se25【角色移动-炮车】）
                                AudioEngine::setVolume(AudioEngine::play2d("move.mp3"), (soundOn ? 1.0f : 0.0f));
                            }
                            else
                            {
                                if (placelm[0] == 81)
                                {
                                    moveai = moveai - 10 + i;
                                }
                                break;
                            }
 
                        }
                        for (int i = 0; i < moveai; i++)
                        {
                            if (placelm[0] != 0 && (placelm[1] == 27 || map[placelm[0] - 1][placelm[1]] == 0))
                            {
                                //困难ai的加强方案：悬崖勒马
                                if (!level && map[placelm[0] - 1][placelm[1] - 1] == 0)
                                {
                                    break;
                                }
                                //简单电脑的移动方案：随机移动
                                move(spritelm, placelm, 1, 1);
                                //移动音效（三国志曹操传：：Se25【角色移动-炮车】）
                                AudioEngine::setVolume(AudioEngine::play2d("move.mp3"), (soundOn ? 1.0f : 0.0f));
                            }
                            else if (climb == 0 && placelm[0] != 0 && (placelm[1] == 26 || map[placelm[0] - 1][placelm[1] + 1] == 0))
                            {
                                //不管简单困难，能往上爬总是好的
                                move(spritelm, placelm, 1, 0);
                                //移动音效（三国志曹操传：：Se25【角色移动-炮车】）
                                AudioEngine::setVolume(AudioEngine::play2d("move.mp3"), (soundOn ? 1.0f : 0.0f));
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    });
                this->addChild(endButton, 1);

                //开火音效（三国志曹操传：：Se_m_01【策略-烈火】）
                AudioEngine::setVolume(AudioEngine::play2d("fire.mp3"), (soundOn ? 1.0f : 0.0f));

                //获取并记录直线滑块的值（0到100）
                float power = static_cast<float>(linearSlider->getPercent());
                //获取并记录圆环滑块的值（0到100）
                float angle = static_cast<float>(radialSlider->getPercent());

                angle = static_cast<float>(180 - 1.8 * angle);
                //发射炮弹
                auto fire = Sprite::create("shell.png");
                if (ai)
                {
                    if (firer == 1)
                    {
                        fire->setPosition(spritely->getPosition());
                    }
                    else
                    {
                        fire->setPosition(spritelm->getPosition());
                    }
                }
                else
                {
                    fire->setPosition(spritely->getPosition());
                }
                //扣减体力
                if (!life)
                {
                    //更新hpLabel的文本内容
                    string hplyStr = "Red's HP: " + to_string(hp.getHply());
                    hplyLabel->setString(hplyStr);
                    string hplmStr = "Blue's HP: " + to_string(hp.getHplm());
                    hplmLabel->setString(hplmStr);
                }
                firer = -firer;
                this->addChild(fire, 0);
                float gravity = -980; //重力加速度
                //计算炮弹的水平速度和垂直速度
                float initialVelocityX = 12 * power * cos(CC_DEGREES_TO_RADIANS(angle));
                float initialVelocityY = 12 * power * sin(CC_DEGREES_TO_RADIANS(angle));

                //创建动作，模拟炮弹的抛物线运动
                auto action = Sequence::create(
                    MoveBy::create(1.0f, Vec2(initialVelocityX, initialVelocityY)),
                    CallFunc::create([=]() {
                        //炮弹运动结束后的回调函数
                        this->removeChild(fire, true);
                        }),
                    nullptr
                );

                //运行动作，并在每一帧更新垂直速度
                fire->runAction(RepeatForever::create(Sequence::create(
                    DelayTime::create(static_cast<float>(0.016)), //帧率为 60 帧/秒，每帧的时间间隔为 1/60 秒
                    CallFunc::create([=]() mutable {
                        initialVelocityY += gravity * static_cast<float>(0.016); //根据重力更新垂直速度
                        Vec2 newPosition = fire->getPosition() + Vec2(initialVelocityX * static_cast<float>(0.016), initialVelocityY * static_cast<float>(0.016)); // 更新炮弹位置
                        //进行越界检测
                        if (newPosition.x + fire->getContentSize().width<0 || newPosition.x - fire->getContentSize().width >visibleSize.width || newPosition.y + fire->getContentSize().height < 0)
                        {
                            //删除炮弹
                            this->removeChild(fire, true);
                            //可以结束阶段
                            endButton->setEnabled(true);
                        }
                        //计算炮弹的边界框
                        Rect fireBoundingBox = Rect(newPosition.x, newPosition.y, fire->getContentSize().width, fire->getContentSize().height);

                        //进行碰撞检测
                        for (int i = 0; i < 83; i++) 
                        {
                            for (int j = 0; j < 27; j++) 
                            {
                                if (map[i][j] == 1) //只检查地面精灵
                                { 
                                    Rect groundBoundingBox = Rect(static_cast<float>(35 + i * 15), static_cast<float>(30 + j * 15), ground[i][j]->getContentSize().width, ground[i][j]->getContentSize().height);
                                    //如果shell与ground[i][j]碰撞
                                    if (fireBoundingBox.intersectsRect(groundBoundingBox)) 
                                    {
                                        //碰撞音效（三国志曹操传：：Se_m_07【策略-山岚】）
                                        AudioEngine::setVolume(AudioEngine::play2d("crash.mp3"), (soundOn ? 1.0f : 0.0f));
                                        //计算扣减生命
                                        if (!life)
                                        {
                                            relife(i, j, placely, placelm, effect, &hp);
                                        }
                                        if (effect)
                                        {
                                            //删除shell和ground[i][j]
                                            this->removeChild(fire, true);
                                            this->removeChild(ground[i][j], true);
                                            //可以结束阶段
                                            endButton->setEnabled(true);
                                            //删除周围的地面精灵
                                            for (int dx = -3; dx <= 3; dx++) 
                                            {
                                                for (int dy = -2; dy <= 2; dy++) 
                                                {
                                                    if (abs(dx) + abs(dy) > 3)
                                                    {
                                                        continue;
                                                    }
                                                    int nx = i + dx;
                                                    int ny = j + dy;
                                                    if (nx >= 0 && nx < 83 && ny >= 0 && ny < 27 && map[nx][ny] == 1)
                                                    {
                                                        this->removeChild(ground[nx][ny], true);
                                                        map[nx][ny] = 0; //标记为已删除
                                                        //进行角色重力检测
                                                        check(map, placely, placelm, spritely, spritelm);
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {
                                            //删除ground[i][j]
                                            this->removeChild(ground[i][j], true);
                                            //可以结束阶段
                                            endButton->setEnabled(true);
                                            //删除周围的地面精灵
                                            map[i][j] = 0; //标记为已删除
                                            //进行角色重力检测
                                            check(map, placely, placelm, spritely, spritelm);
                                        }
                                    }
                                }
                            }
                        }

                        //禁用recordButton，启用moveLeftButton和moveRightButton
                        recordButton->setEnabled(false);
                        moveLeftButton->setEnabled(true);
                        moveRightButton->setEnabled(true);

                        fire->setPosition(newPosition); //更新炮弹位置
                        }),
                    nullptr
                )));

                strengthl = 10, strengthr = 10;
                // 更新strengthLabel的文本内容
                string strengthStrl = "LeftStrength: " + to_string(strengthl);
                strengthLabell->setString(strengthStrl);
                string strengthStrr = "RightStrength: " + to_string(strengthr);
                strengthLabelr->setString(strengthStrr);
                // 更新turnLabel的文本内容
                string turnStr;
                if (ai)
                {
                    if (firer == 1)
                    {
                        turnStr = "now blue can move!";
                    }
                    else
                    {
                        turnStr = "now red can move!";
                    }
                }
                else
                {
                    turnStr = "now red can move!";
                }
                turnLabel->setString(turnStr);
                });
            this->addChild(recordButton);
            }
        );
    }

     return true;
}

//移动函数：包括不同精灵、两个方向、是否攀爬等情况
void HelloWorld::move(Sprite* sprite,int* place,bool leftOrRight,bool climb)//leftOrRight:left1right0
{
    if (leftOrRight)
    {
        if (climb)
        {
            sprite->setPositionX(sprite->getPositionX() - 15);
            place[0]--;
        }
        else
        {
            sprite->setPositionX(sprite->getPositionX() - 15);
            sprite->setPositionY(sprite->getPositionY() + 15);
            place[0]--;
            place[1]++;
        }
    }
    else
    {
        if (climb)
        {
            sprite->setPositionX(sprite->getPositionX() + 15);
            place[0]++;
        }
        else
        {
            sprite->setPositionX(sprite->getPositionX() + 15);
            sprite->setPositionY(sprite->getPositionY() + 15);
            place[0]++;
            place[1]++;
        }
    }
}

//检查重力函数：包括两个精灵
void HelloWorld::check(int map[83][27], int* placely, int* placelm, Sprite* spritely, Sprite* spritelm)
{
    while (map[placely[0]][placely[1] - 1] == 0 && map[placely[0] + 1][placely[1] - 1] == 0)
    {
        spritely->setPositionY(spritely->getPositionY() - 15);
        placely[1]--;
    }
    while (map[placelm[0]][placelm[1] - 1] == 0 && map[placelm[0] + 1][placelm[1] - 1] == 0)
    {
        spritelm->setPositionY(spritelm->getPositionY() - 15);
        placelm[1]--;
    }
}

//损失生命值函数：包括两个精灵，只在开启生命值模式情况被调用，因炮弹模式而有区别
void HelloWorld::relife(int i, int j, int* placely, int* placelm, bool effect, healthpoint* hp)
{
    if (fabs(i - placely[0]) + fabs(j - placely[1]) < 3)
    {
        if (effect)
        {
            hp->hurtHply(10);
        }
        else
        {
            hp->hurtHply(3);
        }
    }
    if (fabs(i - placelm[0]) + fabs(j - placelm[1]) < 3)
    {
        if (effect)
        {
            hp->hurtHplm(10);
        }
        else
        {
            hp->hurtHplm(3);
        }
    }
}

/*
2023.11.27 0.1 新建文件，搭建战斗场景
2023.12.14 0.2 完成开屏界面和开始按钮，重做场景土地
2023.12.15 0.3 完成移动、发射炮弹
2023.12.15 0.4 完成回合制
2023.12.18 0.5 完成角色重力、胜负判定
2023.12.18 0.6 拆分移动与发射，完成音乐与音效
2023.12.19 0.7 完成游戏界面帮助，修复炮弹越界后不触发后续操作的bug
2023.12.20 1.0 完成音乐音效开关，**基础模式完成开发**
2023.12.20 1.1 完成模式选择，完成模式-地图，完成模式-人机，更新开屏图与胜利图
2023.12.21 1.2 完成模式-困难人机，完成模式-炮弹，完成模式-生命
2023.12.22 1.3 完成模式-攀爬，完成开始界面帮助，**基本完成功能开发**
2023.12.22 1.3.1 重写move函数
2023.12.23 1.3.2 重写check函数
2023.12.23 1.3.3 重写relife函数
2023.12.25 1.3.4 删除冗余代码
2023.12.25 1.3.5 重写healthpoint类
2023.12.25 1.3.6 处理智能warning
2023.12.31 1.3.7 修改healthpoint类
*/

//D:\learn cocos\ddt\proj.win32\bin\ddt\Debug