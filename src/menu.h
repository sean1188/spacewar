#pragma once

#include "common.h"
#include "constants.h"
#include "input.h"
#include "math/lerp.h"
#include "scene.h"
#include "stage.h"
#include "systems/animation.h"

struct CMenuShipSelectControlled : Component<CMenuShipSelectControlled> {
};

class SMenuShipSelect : public System
{
  Input* input;
  Stage* stageScene;
  Game* game;

public:
  SMenuShipSelect(Input* _input, Stage* _stageScene, Game* _game) : System()
  {
    System::addComponentType(CMenuShipSelectControlled::id);
    System::addComponentType(CSprite::id);

    this->input = _input;
    this->stageScene = _stageScene;
    this->game = _game;
  }
  virtual void updateComponents(float delta, BaseComponent** components)
  {
    CMenuShipSelectControlled* controlComponent =
        (CMenuShipSelectControlled*)components[0];
    CSprite* shipSprite = (CSprite*)components[1];

    if (input->getKeyboardKeyState(VK_LEFT) == JustPressed) {
		if (shipSprite->currentFrame > shipSprite->startFrame)
			PlaySound(CLICK_AUDIO, NULL, SND_FILENAME | SND_ASYNC);
			shipSprite->currentFrame -= 1;
		shipSprite->setRect();
    } else if (input->getKeyboardKeyState(VK_RIGHT) == JustPressed) {
		if (shipSprite->currentFrame < shipSprite->endFrame)
			PlaySound(CLICK_AUDIO, NULL, SND_FILENAME | SND_ASYNC);
			shipSprite->currentFrame += 1;
		shipSprite->setRect();
    }

    if (input->getKeyboardKeyState(VK_SPACE) == JustPressed) 
	{
		PlaySound(CHOOSE_SPACESHIP_AUDIO, NULL, SND_FILENAME | SND_SYNC);
		game->setScene(stageScene);
    }
  }
};

class Menu : public Scene
{

  TextureManager backgroundTexture, spaceShipTexture, angleIconTexture,
      promptTexture, gameTitleTexture;

  // Systems
  SAnimation* menuAnimation;
  SMenuShipSelect* menuShipSelect;

  // Components
  CSprite backgroundImage, shipSprite, rightKeySprite, leftKeySprite,
      promptSprite, titleSprite;
  CAnimated shipAnimation;
  CMenuShipSelectControlled shipSelectControls;

  // Entities
  EntityHook background;
  EntityHook spaceship;
  EntityHook title;

  Stage* stageScene = new Stage();

public:
  Menu() : Scene() 
  {
	  PlaySound(MENU_SCREEN_AUDIO, NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
  }
  ~Menu()
  {
    backgroundTexture.onLostDevice();
    backgroundTexture.onResetDevice();
    spaceShipTexture.onLostDevice();
    spaceShipTexture.onResetDevice();
    angleIconTexture.onLostDevice();
    angleIconTexture.onResetDevice();
    promptTexture.onLostDevice();
    promptTexture.onResetDevice();
    gameTitleTexture.onLostDevice();
    gameTitleTexture.onResetDevice();
  }

  void setupSystems()
  {
    menuAnimation = new SAnimation();
    menuShipSelect = new SMenuShipSelect(input, stageScene, game);
  }

  void setupTextures()
  {
    if (!backgroundTexture.initialise(graphics, BACKGROUND))
      Logger::error("Failed to load background texture");
    if (!spaceShipTexture.initialise(graphics, SHIPS))
      Logger::error("Failed to load ships texture");
    if (!angleIconTexture.initialise(graphics, ANGLE_ICON))
      Logger::error("Failed to load angle icons texture");
    if (!promptTexture.initialise(graphics, PROMPT))
      Logger::error("Failed to load spacebar texture");
    if (!gameTitleTexture.initialise(graphics, GAME_LOGO))
      Logger::error("Failed to load game title texture");
  }

  void setupEntities()
  {

    backgroundImage.startFrame = 0, backgroundImage.endFrame = 0,
    backgroundImage.currentFrame = 0;
    backgroundImage.animates = false;
    backgroundImage.initialise(BACKGROUND_WIDTH, BACKGROUND_HEIGHT,
                               BACKGROUND_COLS, &backgroundTexture);
    backgroundImage.setScale(0.5);
    backgroundImage.setPosition(0, 0);

    shipSprite.startFrame = 0, shipSprite.endFrame = 9,
    shipSprite.currentFrame = 0;
    shipSprite.animates = false;
    shipSprite.initialise(SHIPS_WIDTH, SHIPS_HEIGHT, SHIPS_COLS,
                          &spaceShipTexture);
    shipSprite.setScale(1.5);
    shipSprite.setPosition(GAME_WIDTH / 2 - shipSprite.getWidth() / 2,
                           (GAME_HEIGHT / 2 - shipSprite.getHeight() / 2) + 70);
    shipAnimation.animationType = SCALE;
    shipAnimation.startValue = 1.5, shipAnimation.endValue = 1.7,
    shipAnimation.reverses = true, shipAnimation.rate = 0.06;

    // Setup GUI, etc
    titleSprite.startFrame = 0, titleSprite.endFrame = 2;
    titleSprite.currentFrame = 0;
    titleSprite.animates = true;
    titleSprite.initialise(GAME_LOGO_WIDTH, GAME_LOGO_HEIGHT, GAME_LOGO_COLS,
                           &gameTitleTexture);
    titleSprite.frameDelay = 0.1;
    titleSprite.setScale(0.7);
    titleSprite.setPosition(GAME_WIDTH / 2 - titleSprite.getWidth() / 2, 50);

    rightKeySprite.currentFrame = 1;
    rightKeySprite.animates = false;
    rightKeySprite.initialise(ANGLE_ICON_WIDTH, ANGLE_ICON_HEIGHT,
                              ANGLE_ICON_COLS, &angleIconTexture);
    rightKeySprite.setScale(0.5);
    rightKeySprite.setPosition(shipSprite.getX() + 100 + shipSprite.getWidth(),
                               shipSprite.getY() + rightKeySprite.getHeight());

    leftKeySprite.currentFrame = 0;
    leftKeySprite.animates = false;
    leftKeySprite.initialise(ANGLE_ICON_WIDTH, ANGLE_ICON_HEIGHT,
                             ANGLE_ICON_COLS, &angleIconTexture);
    leftKeySprite.setScale(0.5);
    leftKeySprite.setPosition(shipSprite.getX() - 100 -
                                  leftKeySprite.getWidth(),
                              shipSprite.getY() + leftKeySprite.getHeight());

    promptSprite.startFrame = 0, promptSprite.endFrame = 0,
    promptSprite.currentFrame = 0;
    promptSprite.animates = false;
    promptSprite.initialise(PROMPT_WIDTH, PROMPT_HEIGHT, PROMPT_COLS,
                            &promptTexture);
    promptSprite.setScale(0.7);
    promptSprite.setPosition(GAME_WIDTH / 2 - promptSprite.getWidth() / 2,
                             GAME_HEIGHT / 2 + promptSprite.getHeight() * 3);
  }

  void render()
  {
    leftKeySprite.spriteData.texture =
        leftKeySprite.textureManager->getTexture();

    rightKeySprite.spriteData.texture =
        rightKeySprite.textureManager->getTexture();

    promptSprite.spriteData.texture = promptSprite.textureManager->getTexture();

    graphics->spriteBegin();

    graphics->drawSprite(leftKeySprite.spriteData);
    graphics->drawSprite(rightKeySprite.spriteData);
    graphics->drawSprite(promptSprite.spriteData);

    graphics->spriteEnd();
  }

  void attach()
  {
    gameSystems->addSystem(*menuAnimation);
    gameSystems->addSystem(*menuShipSelect);
    background = ecs->makeEntity(backgroundImage);
    spaceship = ecs->makeEntity(shipSprite, shipAnimation, shipSelectControls);
    title = ecs->makeEntity(titleSprite);
    Scene::attach();
  }

  void detach()
  {

    gameSystems->removeSystem(*menuAnimation);
    gameSystems->removeSystem(*menuShipSelect);
    ecs->removeEntity(background);
    ecs->removeEntity(spaceship);
    ecs->removeEntity(title);
    Scene::detach();
  }
};
