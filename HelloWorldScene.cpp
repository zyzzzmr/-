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

//2150228 ������

#include "HelloWorldScene.h"
#include "ui/CocosGUI.h"
#include "audio/include/AudioEngine.h"
using namespace cocos2d::ui;
using namespace std;
/* ��עһЩ���ò���
��Ļ��С��1300*800�����أ�
��ɫ��ͼ��С��75*70�����أ�
ǽ��ͼ��С��40*40�����أ�
������ͼ��С��25*25�����أ�
���ز��ִ�С��83*27����)
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
    // ����
    const auto visibleSize = Size(1300, 800);
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // һЩ����
    // ģʽ����
    static bool effect = 1;//�ڵ���1==��ը����0==���׵�
    static bool life = 1;//������1==���ޣ�0==����
    static bool climb = 1;//������1==�أ�0==��
    static bool ai = 1;//���֣�1==������0==�˻�
    static bool level = 1;//�˻��Ѷȣ�1==�򵥣�0==����
    static bool geography = 1;//��ͼ��1==��ͨ��0==����
    //�������꣨�ؿ��棩
    static int placely[2] = { 19,27 };
    static int placelm[2] = { 62,27 };

    static healthpoint hp;
    //��������Ч����
    static bool musicOn = false;
    static bool soundOn = false;
    //��ͼ
    static int map[83][27];
    //��������
    static int help = 1;
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    /////////////////////////////
    // 3. add your codes below...
    /***************************��ʼ����***************************/
    //����
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

    //�������ְ�ť
    int bgmID = -1;
    auto musicButton = Button::create("MusicOff.png", "MusicOn.png");
    musicButton->setPosition(Vec2(visibleSize.width - 100, visibleSize.height - 50));
    musicButton->addClickEventListener([=](Ref* sender) mutable {
        musicOn = !musicOn;
        if (musicOn) 
        {
            musicButton->loadTextures("MusicOn.png", "MusicOff.png");
            //���ű������֣�����־�ܲٴ�����Se_e_07����������-ս�ҡ���
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
            //��ͣ��������
            if (bgmID != -1) 
            {
                AudioEngine::pause(bgmID);
            }
        }
        });
    this->addChild(musicButton);

    //������Ч��ť
    auto soundButton = Button::create("SoundOff.png", "SoundOn.png");
    soundButton->setPosition(Vec2(visibleSize.width - 50, visibleSize.height - 50));
    soundButton->addClickEventListener([=](Ref* sender) mutable {
        soundOn = !soundOn;
        if (soundOn) 
        {
            soundButton->loadTextures("SoundOn.png", "SoundOff.png");
            //������Ч
        }
        else 
        {
            soundButton->loadTextures("SoundOff.png", "SoundOn.png");
            //�ر���Ч
        }
        });

    this->addChild(soundButton);
    //����������ť
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
    //��������ͼ��
    bool isHelperVisible = false; //��־helper�Ƿ�ɼ�
    Node* helperContainer = nullptr; //���ڰ�װhelper�������ڵ�
    helpButton->addClickEventListener([=](Ref* sender) mutable {
        if (!isHelperVisible) 
        {
            //���helper���ɼ����򴴽���������������ڵ�
            if (help)//��ʾ��ʼ�������
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
            else//��ʾ��Ϸ�������
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
            //���helper�ѿɼ�����������ڵ����Ƴ���
            if (helperContainer != nullptr && helperContainer->getChildrenCount() > 0) 
            {
                helperContainer->removeAllChildren();
            }
            isHelperVisible = false;
        }
        });

    /***************************�˵�*********************************/
    //�ڵ�ѡ��
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
    //����ѡ��
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
    //����ѡ��
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
    //����ѡ��
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
    //�˻��Ѷ�ѡ��
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
    //��ͼѡ��
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
    //������ʼ��ť���飨����־�ܲٴ����������ڣ�
    auto startButton = Button::create("starter0.png", "starter.png", "0.png");
    if (startButton == nullptr)
    {
        problemLoading("'starter0.png', or'starter.png'");
    }
    else
    {
        startButton->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        this->addChild(startButton, 3);

        //ע�������
        startButton->addClickEventListener([=](Ref* sender) mutable {

            //ɾ����ʼ����
            this->removeChild(startButton, true);
            this->removeChild(background, true);
            this->removeChild(effectButton, true);
            this->removeChild(lifeButton, true);
            this->removeChild(climbButton, true);
            this->removeChild(aiButton, true);
            this->removeChild(levelButton, true);
            this->removeChild(geographyButton, true);

            /*=======================��ʼ��===============================*/
            //������������
            help = 0;
            //��Ϸ����
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
            //�췽��ɫ������־�ܲٴ��������ʣ�//ly=>liuye
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
            //������ɫ������־��ά������������//lm=>liumin
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

            //ǽ���ϣ�����־�ܲٴ�������ǽ��//u.d.l.r=>up.down.left.right
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

            //ǽ���ϣ�����־�ܲٴ�������ǽ��
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

            //ǽ���£�����־�ܲٴ�������ǽ��
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

            //ǽ���£�����־�ܲٴ�������ǽ��
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

            //ǽ�ϣ�����־�ܲٴ�������ǽ��
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

            //ǽ�£�����־�ܲٴ�������ǽ��
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

            //ǽ������־�ܲٴ�������ǽ��
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

            //ǽ�ң�����־�ܲٴ�������ǽ��
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

            //����
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
            /*****************************��Ϸ*********************************/
            //��ǰ�غϽ�ɫ
            int firer = 1;
            //strength���ӻ�
            Label* strengthLabell;
            strengthLabell = Label::createWithTTF("LeftStrength: 10", "fonts/arial.ttf", 24);
            strengthLabell->setPosition(Vec2(100, visibleSize.height - 150));
            this->addChild(strengthLabell, 1);
            Label* strengthLabelr;
            strengthLabelr = Label::createWithTTF("RightStrength: 10", "fonts/arial.ttf", 24);
            strengthLabelr->setPosition(Vec2(100, visibleSize.height - 175));
            this->addChild(strengthLabelr, 1);
            //hp���ӻ�
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
            //turn���ӻ�
            Label* turnLabel;
            turnLabel = Label::createWithTTF("now red can fire!", "fonts/arial.ttf", 24);
            turnLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 150));
            this->addChild(turnLabel, 1);

            //�������Ȼ���
            auto linearSlider = Slider::create();
            linearSlider->loadBarTexture("arrow0.png");
            linearSlider->loadSlidBallTextures("shell.png", "shell1.png");
            linearSlider->setPercent(50); // ���ó�ʼֵ��0��100��
            linearSlider->setPosition(Vec2(300, 700));
            this->addChild(linearSlider);
            //�����ǶȻ���
            auto radialSlider = Slider::create();
            radialSlider->loadBarTexture("angleer.png");
            radialSlider->loadSlidBallTextures("shell.png", "shell1.png");
            radialSlider->setPercent(50); // ���ó�ʼֵ��0��100��
            radialSlider->setPosition(Vec2(500, 700));
            this->addChild(radialSlider);

            //��������ť
            auto recordButton = Button::create("shell.png", "shell1.png", "0.png");
            recordButton->setPosition(Vec2(400, 650));
            recordButton->addClickEventListener([=](Ref* sender) mutable {
                int strengthl = 10, strengthr = 10;

                //�������ư�ť
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
                                    //����"spritely"����25
                                    move(spritely, placely, 1, 1);
                                }
                                else if (firer == -1 && placelm[0] != 0 && (placelm[1] == 27 || map[placelm[0] - 1][placelm[1]] == 0))
                                {
                                    //����"spritelm"����25
                                    move(spritelm, placelm, 1, 1);
                                }
                                else if (climb == 0 && firer == 1 && placely[0] != 0 && (placely[1] == 26 || map[placely[0] - 1][placely[1] + 1] == 0))
                                {
                                    //����"spritely"������25
                                    move(spritely, placely, 1, 0);
                                }
                                else if (climb == 0 && firer == -1 && placelm[0] != 0 && (placelm[1] == 26 || map[placelm[0] - 1][placelm[1] + 1] == 0))
                                {
                                    //����"spritelm"������25
                                    move(spritelm, placelm, 1, 0);
                                }
                            }
                            else
                            {
                                if (placely[0] != 0 && (placely[1] == 27 || map[placely[0] - 1][placely[1]] == 0))
                                {
                                    //����"spritely"����25
                                    move(spritely, placely, 1, 1);
                                }
                                else if (climb == 0 && placely[0] != 0 && (placely[1] == 26 || map[placely[0] - 1][placely[1] + 1] == 0))
                                {
                                    //����"spritely"������25
                                    move(spritely, placely, 1, 0);
                                }
                            }
                            //strength����
                            strengthl--;
                            //����strengthLabel���ı�����
                            string strengthStrl = "LeftStrength: " + to_string(strengthl);
                            strengthLabell->setString(strengthStrl);
                            //���н�ɫ�������
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
                            //�ƶ���Ч������־�ܲٴ�����Se25����ɫ�ƶ�-�ڳ�����
                            AudioEngine::setVolume(AudioEngine::play2d("move.mp3"), (soundOn ? 1.0f : 0.0f));
                        }
                        });
                }
                //�������ư�ť
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
                                    //����"spritely"����25
                                    move(spritely, placely, 0, 1);
                                }
                                else if (firer == -1 && placelm[0] != 81 && (placelm[1] == 27 || map[placelm[0] + 2][placelm[1]] == 0))
                                {
                                    //����"spritelm"����25
                                    move(spritelm, placelm, 0, 1);
                                }
                                else if (climb == 0 && firer == 1 && placely[0] != 81 && (placely[1] == 26 || map[placely[0] + 2][placely[1] + 1] == 0))
                                {
                                    //����"spritely"������25
                                    move(spritely, placely, 0, 0);
                                }
                                else if (climb == 0 && firer == -1 && placelm[0] != 81 && (placelm[1] == 26 || map[placelm[0] + 2][placelm[1] + 1] == 0))
                                {
                                    //����"spritelm"������25
                                    move(spritelm, placelm, 0, 0);
                                }
                            }
                            else
                            {
                                if (placely[0] != 81 && (placely[1] == 27 || map[placely[0] + 2][placely[1]] == 0))
                                {
                                    //����"spritely"����25
                                    move(spritely, placely, 0, 1);
                                }
                                else if (climb == 0 && placely[0] != 81 && (placely[1] == 26 || map[placely[0] + 2][placely[1] + 1] == 0))
                                {
                                    //����"spritely"������25
                                    move(spritely, placely, 0, 0);
                                }
                            }
                            //strength����
                            strengthr--;
                            //����strengthLabel���ı�����
                            string strengthStrr = "RightStrength: " + to_string(strengthr);
                            strengthLabelr->setString(strengthStrr);
                            //���н�ɫ�������
                            check(map, placely, placelm, spritely, spritelm);
                            AudioEngine::setVolume(AudioEngine::play2d("move.mp3"), (soundOn ? 1.0f : 0.0f));
                        }
                        });
                }
                //���������غϰ�ť
                auto endButton = Button::create("ender.png", "starter.png", "0.png");
                endButton->setPosition(Vec2(visibleSize.width / 2, 700));
                endButton->addClickEventListener([=](Ref* sender) mutable {
                    //����recordButton������moveLeftButton��moveRightButton
                    recordButton->setEnabled(true);
                    moveLeftButton->setEnabled(false);
                    moveRightButton->setEnabled(false);
                    //����turnLabel���ı�����
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
                    //�ۼ�����
                    if (!life)
                    {
                        //����hpLabel���ı�����
                        string hplyStr = "Red's HP: " + to_string(hp.getHply());
                        hplyLabel->setString(hplyStr);
                        string hplmStr = "Blue's HP: " + to_string(hp.getHplm());
                        hplmLabel->setString(hplmStr);
                    }
                    //����ʤ�����
                    if (placely[1] <= 0 ||(!life && hp.getHply() <= 0))
                    {
                        //ʤ����Ч������־�ܲٴ�����Se13����ɫ-��������
                        AudioEngine::play2d("win.mp3");
                        //����ʤ�����棨����ɱonline��������ͼ-��������
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
                        //ʤ����Ч������־�ܲٴ�����Se13����ɫ-��������
                        AudioEngine::play2d("win.mp3");
                        //����ʤ�����棨����ɱonline��������ͼ-��������
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
                    //��սai
                    if (!ai)
                    {
                        firer = -firer;
                        float powerai, angleai;
                        if (level)
                        {
                            //�򵥵��ԵĿ��𷽰����������
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
                            //���ѵ��ԵĿ��𷽰����ڽǹ̶�60�㣬���Ƚⷽ�̵ó�
                            if (placely[0] < placelm[0])
                            {
                                angleai = 120;
                            }
                            else
                            {
                                angleai = 60;
                            }
                            powerai = static_cast<float>(fabs(spritely->getPosition().x - spritelm->getPosition().x) / (6 * sqrt((sqrt(3) * fabs(spritely->getPosition().x - spritelm->getPosition().x) - spritely->getPosition().y + spritelm->getPosition().y) / 490)));
                            //�ر�ע�ͣ���ά�ȼ���ֱ���˶������������t��powerai���ʽ
                        }
                        auto fire = Sprite::create("shell.png");
                        fire->setPosition(spritelm->getPosition());
                        this->addChild(fire, 0);

                        //������Ч������־�ܲٴ�����Se_m_01������-�һ𡿣�
                        AudioEngine::setVolume(AudioEngine::play2d("fire.mp3"), (soundOn ? 1.0f : 0.0f));

                        float gravity = -980; //�������ٶ�
                        //�����ڵ���ˮƽ�ٶȺʹ�ֱ�ٶ�
                        float initialVelocityX = 12 * powerai * cos(CC_DEGREES_TO_RADIANS(angleai));
                        float initialVelocityY = 12 * powerai * sin(CC_DEGREES_TO_RADIANS(angleai));

                        //����������ģ���ڵ����������˶�

                        auto action = Sequence::create(
                            MoveBy::create(1.0f, Vec2(initialVelocityX, initialVelocityY)),
                            CallFunc::create([=]() {
                                //�ڵ��˶�������Ļص�����
                                this->removeChild(fire, true);
                                }),
                            nullptr
                        );
                        //���ж���������ÿһ֡���´�ֱ�ٶ�
                        fire->runAction(RepeatForever::create(Sequence::create(
                            DelayTime::create(static_cast<float>(0.016)), //֡��Ϊ 60 ֡/�룬ÿ֡��ʱ����Ϊ 1/60 ��
                            CallFunc::create([=]() mutable {
                                initialVelocityY += gravity * static_cast<float>(0.016); //�����������´�ֱ�ٶ�
                                Vec2 newPosition = fire->getPosition() + Vec2(initialVelocityX * static_cast<float>(0.016), initialVelocityY * static_cast<float>(0.016)); // �����ڵ�λ��
                                //����Խ����
                                if (newPosition.x + fire->getContentSize().width<0 || newPosition.x - fire->getContentSize().width >visibleSize.width || newPosition.y + fire->getContentSize().height < 0)
                                {
                                    //ɾ���ڵ�
                                    this->removeChild(fire, true);
                                    //���Խ����׶�
                                    endButton->setEnabled(true);
                                }
                                //�����ڵ��ı߽��
                                Rect fireBoundingBox = Rect(newPosition.x, newPosition.y, fire->getContentSize().width, fire->getContentSize().height);

                                //������ײ���
                                for (int i = 0; i < 83; i++) 
                                {
                                    for (int j = 0; j < 27; j++) 
                                    {
                                        if (map[i][j] == 1) //ֻ�����澫��
                                        { 
                                            Rect groundBoundingBox = Rect(static_cast<float>(35 + i * 15), static_cast<float>(30 + j * 15), ground[i][j]->getContentSize().width, ground[i][j]->getContentSize().height);
                                            //shell��ground[i][j]��ײ
                                            if (fireBoundingBox.intersectsRect(groundBoundingBox)) 
                                            {
                                                //��ײ��Ч������־�ܲٴ�����Se_m_07������-ɽᰡ���
                                                AudioEngine::setVolume(AudioEngine::play2d("crash.mp3"), (soundOn ? 1.0f : 0.0f));
                                                //����ۼ�����
                                                if (!life)
                                                {
                                                    relife(i, j, placely, placelm, effect, &hp);
                                                }
                                                if (effect)
                                                {
                                                    //ɾ�� shell �� ground[i][j]
                                                    this->removeChild(fire, true);
                                                    this->removeChild(ground[i][j], true);
                                                    //���Խ����׶�
                                                    endButton->setEnabled(true);
                                                    //ɾ����Χ�ĵ��澫��
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
                                                                map[nx][ny] = 0; //���Ϊ��ɾ��
                                                                //���н�ɫ�������
                                                                check(map, placely, placelm, spritely, spritelm);
                                                            }
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    //ɾ��ground[i][j]
                                                    this->removeChild(ground[i][j], true);
                                                    //���Խ����׶�
                                                    endButton->setEnabled(true);
                                                    //ɾ����Χ�ĵ��澫��
                                                    map[i][j] = 0; // ���Ϊ��ɾ��
                                                    //���н�ɫ�������
                                                    check(map, placely, placelm, spritely, spritelm);
                                                }
                                            }
                                        }
                                    }
                                }

                                fire->setPosition(newPosition); //�����ڵ�λ��
                                }),
                            nullptr
                        )));
                        int moveai = (rand() % 21);
                        for (int i = 0; i < 10; i++)
                        {
                            //��������ai��˵��������̶��˷���Ƕȣ��������ұ����ǱȽϺ���ģ����ڼ�ai���ԣ����Ҵ�������ν����֮����������
                            if (placelm[0] != 81 && (placelm[1] == 27 || map[placelm[0] + 2][placelm[1]] == 0))
                            {
                                //����ai�ļ�ǿ��������������
                                if (!level && map[placelm[0] + 2][placelm[1] - 1] == 0)
                                {
                                    break;
                                }
                                //�򵥵��Ե��ƶ�����������ƶ�
                                move(spritelm, placelm, 0, 1);
                                //�ƶ���Ч������־�ܲٴ�����Se25����ɫ�ƶ�-�ڳ�����
                                AudioEngine::setVolume(AudioEngine::play2d("move.mp3"), (soundOn ? 1.0f : 0.0f));
                            }
                            else if (climb == 0 && placelm[0] != 81 && (placelm[1] == 26 || map[placelm[0] + 2][placelm[1] + 1] == 0))
                            {
                                //���ܼ����ѣ������������Ǻõ�
                                move(spritelm, placelm, 0, 0);
                                //�ƶ���Ч������־�ܲٴ�����Se25����ɫ�ƶ�-�ڳ�����
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
                                //����ai�ļ�ǿ��������������
                                if (!level && map[placelm[0] - 1][placelm[1] - 1] == 0)
                                {
                                    break;
                                }
                                //�򵥵��Ե��ƶ�����������ƶ�
                                move(spritelm, placelm, 1, 1);
                                //�ƶ���Ч������־�ܲٴ�����Se25����ɫ�ƶ�-�ڳ�����
                                AudioEngine::setVolume(AudioEngine::play2d("move.mp3"), (soundOn ? 1.0f : 0.0f));
                            }
                            else if (climb == 0 && placelm[0] != 0 && (placelm[1] == 26 || map[placelm[0] - 1][placelm[1] + 1] == 0))
                            {
                                //���ܼ����ѣ������������Ǻõ�
                                move(spritelm, placelm, 1, 0);
                                //�ƶ���Ч������־�ܲٴ�����Se25����ɫ�ƶ�-�ڳ�����
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

                //������Ч������־�ܲٴ�����Se_m_01������-�һ𡿣�
                AudioEngine::setVolume(AudioEngine::play2d("fire.mp3"), (soundOn ? 1.0f : 0.0f));

                //��ȡ����¼ֱ�߻����ֵ��0��100��
                float power = static_cast<float>(linearSlider->getPercent());
                //��ȡ����¼Բ�������ֵ��0��100��
                float angle = static_cast<float>(radialSlider->getPercent());

                angle = static_cast<float>(180 - 1.8 * angle);
                //�����ڵ�
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
                //�ۼ�����
                if (!life)
                {
                    //����hpLabel���ı�����
                    string hplyStr = "Red's HP: " + to_string(hp.getHply());
                    hplyLabel->setString(hplyStr);
                    string hplmStr = "Blue's HP: " + to_string(hp.getHplm());
                    hplmLabel->setString(hplmStr);
                }
                firer = -firer;
                this->addChild(fire, 0);
                float gravity = -980; //�������ٶ�
                //�����ڵ���ˮƽ�ٶȺʹ�ֱ�ٶ�
                float initialVelocityX = 12 * power * cos(CC_DEGREES_TO_RADIANS(angle));
                float initialVelocityY = 12 * power * sin(CC_DEGREES_TO_RADIANS(angle));

                //����������ģ���ڵ����������˶�
                auto action = Sequence::create(
                    MoveBy::create(1.0f, Vec2(initialVelocityX, initialVelocityY)),
                    CallFunc::create([=]() {
                        //�ڵ��˶�������Ļص�����
                        this->removeChild(fire, true);
                        }),
                    nullptr
                );

                //���ж���������ÿһ֡���´�ֱ�ٶ�
                fire->runAction(RepeatForever::create(Sequence::create(
                    DelayTime::create(static_cast<float>(0.016)), //֡��Ϊ 60 ֡/�룬ÿ֡��ʱ����Ϊ 1/60 ��
                    CallFunc::create([=]() mutable {
                        initialVelocityY += gravity * static_cast<float>(0.016); //�����������´�ֱ�ٶ�
                        Vec2 newPosition = fire->getPosition() + Vec2(initialVelocityX * static_cast<float>(0.016), initialVelocityY * static_cast<float>(0.016)); // �����ڵ�λ��
                        //����Խ����
                        if (newPosition.x + fire->getContentSize().width<0 || newPosition.x - fire->getContentSize().width >visibleSize.width || newPosition.y + fire->getContentSize().height < 0)
                        {
                            //ɾ���ڵ�
                            this->removeChild(fire, true);
                            //���Խ����׶�
                            endButton->setEnabled(true);
                        }
                        //�����ڵ��ı߽��
                        Rect fireBoundingBox = Rect(newPosition.x, newPosition.y, fire->getContentSize().width, fire->getContentSize().height);

                        //������ײ���
                        for (int i = 0; i < 83; i++) 
                        {
                            for (int j = 0; j < 27; j++) 
                            {
                                if (map[i][j] == 1) //ֻ�����澫��
                                { 
                                    Rect groundBoundingBox = Rect(static_cast<float>(35 + i * 15), static_cast<float>(30 + j * 15), ground[i][j]->getContentSize().width, ground[i][j]->getContentSize().height);
                                    //���shell��ground[i][j]��ײ
                                    if (fireBoundingBox.intersectsRect(groundBoundingBox)) 
                                    {
                                        //��ײ��Ч������־�ܲٴ�����Se_m_07������-ɽᰡ���
                                        AudioEngine::setVolume(AudioEngine::play2d("crash.mp3"), (soundOn ? 1.0f : 0.0f));
                                        //����ۼ�����
                                        if (!life)
                                        {
                                            relife(i, j, placely, placelm, effect, &hp);
                                        }
                                        if (effect)
                                        {
                                            //ɾ��shell��ground[i][j]
                                            this->removeChild(fire, true);
                                            this->removeChild(ground[i][j], true);
                                            //���Խ����׶�
                                            endButton->setEnabled(true);
                                            //ɾ����Χ�ĵ��澫��
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
                                                        map[nx][ny] = 0; //���Ϊ��ɾ��
                                                        //���н�ɫ�������
                                                        check(map, placely, placelm, spritely, spritelm);
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {
                                            //ɾ��ground[i][j]
                                            this->removeChild(ground[i][j], true);
                                            //���Խ����׶�
                                            endButton->setEnabled(true);
                                            //ɾ����Χ�ĵ��澫��
                                            map[i][j] = 0; //���Ϊ��ɾ��
                                            //���н�ɫ�������
                                            check(map, placely, placelm, spritely, spritelm);
                                        }
                                    }
                                }
                            }
                        }

                        //����recordButton������moveLeftButton��moveRightButton
                        recordButton->setEnabled(false);
                        moveLeftButton->setEnabled(true);
                        moveRightButton->setEnabled(true);

                        fire->setPosition(newPosition); //�����ڵ�λ��
                        }),
                    nullptr
                )));

                strengthl = 10, strengthr = 10;
                // ����strengthLabel���ı�����
                string strengthStrl = "LeftStrength: " + to_string(strengthl);
                strengthLabell->setString(strengthStrl);
                string strengthStrr = "RightStrength: " + to_string(strengthr);
                strengthLabelr->setString(strengthStrr);
                // ����turnLabel���ı�����
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

//�ƶ�������������ͬ���顢���������Ƿ����������
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

//�������������������������
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

//��ʧ����ֵ�����������������飬ֻ�ڿ�������ֵģʽ��������ã����ڵ�ģʽ��������
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
2023.11.27 0.1 �½��ļ����ս������
2023.12.14 0.2 ��ɿ�������Ϳ�ʼ��ť��������������
2023.12.15 0.3 ����ƶ��������ڵ�
2023.12.15 0.4 ��ɻغ���
2023.12.18 0.5 ��ɽ�ɫ������ʤ���ж�
2023.12.18 0.6 ����ƶ��뷢�䣬�����������Ч
2023.12.19 0.7 �����Ϸ����������޸��ڵ�Խ��󲻴�������������bug
2023.12.20 1.0 ���������Ч���أ�**����ģʽ��ɿ���**
2023.12.20 1.1 ���ģʽѡ�����ģʽ-��ͼ�����ģʽ-�˻������¿���ͼ��ʤ��ͼ
2023.12.21 1.2 ���ģʽ-�����˻������ģʽ-�ڵ������ģʽ-����
2023.12.22 1.3 ���ģʽ-��������ɿ�ʼ���������**������ɹ��ܿ���**
2023.12.22 1.3.1 ��дmove����
2023.12.23 1.3.2 ��дcheck����
2023.12.23 1.3.3 ��дrelife����
2023.12.25 1.3.4 ɾ���������
2023.12.25 1.3.5 ��дhealthpoint��
2023.12.25 1.3.6 ��������warning
2023.12.31 1.3.7 �޸�healthpoint��
*/

//D:\learn cocos\ddt\proj.win32\bin\ddt\Debug