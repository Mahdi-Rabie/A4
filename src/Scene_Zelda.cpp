///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\
//
//  Assignment       COMP4300 - Assignment 4
//  Professor:       David Churchill
//  Year / Term:     2021FALL
//  File Name:       Scene_Zelda.cpp
// 
//  Student Name:    Mahdi Rabie, Constance Gray
//  Student User:    mr1826, cdg165
//  Student Email:   mr1826@mun.ca, cdg165@mun.ca
//  Student ID:      201025756, 201050531
//  Group Member(s): [enter student name(s)]
//
///\/\\////\///////\////\\\\\\/\\/\/\\////\///////\////\\\\\\/\\

#include "Scene_Zelda.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
                              
Scene_Zelda::Scene_Zelda(GameEngine* game, const std::string& levelPath)
    : Scene(game)
    , m_levelPath(levelPath)
{
    init(m_levelPath);
}

void Scene_Zelda::init(const std::string& levelPath)
{
    loadLevel(levelPath);

    // STUDENT TODO:
    // Register the actions required to play the game

    registerAction(sf::Keyboard::Escape, "QUIT");                       //  Exit to menu if in game and exit window if in menu
    registerAction(sf::Keyboard::P, "PAUSE");                               //  Pause the game
    registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW");          //  Toggle follow camera
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");         //  Toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");      //  Toggle drawing (C)ollision Boxes
	registerAction ( sf::Keyboard::W, "UP" );                                //  Go Up/Jumping
	registerAction ( sf::Keyboard::A, "LEFT" );                              //  Go Left
	registerAction ( sf::Keyboard::S, "DOWN" );                          //  Go Down
    registerAction ( sf::Keyboard::D, "RIGHT" );                          //   Go Right
    registerAction ( sf::Keyboard::Space, "ATTACK" );                //   Player is attacking
}

//  Read in the level file and assign the assets to their starting positions
void Scene_Zelda::loadLevel(const std::string& filename)
{
    // Reset the entity manager every time we load a level
    m_entityManager = EntityManager();
	//  Prepare to read in the level file
	std::ifstream fin ( filename );
	std::string line;

	//  Read in the level file and assign the attributes
	while ( std::getline ( fin, line ) )
	{
		std::istringstream iss ( line );
		std::string firstWord;
		std::string animName, behaviourName;
		int posX, posY, roomX, roomY, blockMove, blockVis, maxHealth, damage, numPatrolSpots, patrolPosX , patrolPosY;
        float patrolSpeed, followSpeed;

		iss >> firstWord;

		if ( firstWord == "Player" )
		{
            iss >> m_playerConfig.X >> m_playerConfig.Y >>
			           m_playerConfig.CX >> m_playerConfig.CY >>
				       m_playerConfig.SPEED >> m_playerConfig.HEALTH;
        }

		else if ( firstWord == "Tile" )
		{  
			iss >> animName >> roomX >> roomY>>posX>>posY>>blockMove>>blockVis;
			auto e = m_entityManager.addEntity ( "tile" );
            e->addComponent<CAnimation> ( m_game->assets ().getAnimation ( animName ), true );
			e->addComponent<CTransform> ( getPosition (roomX, roomY, posX, posY) );			
			e->addComponent<CBoundingBox> ( (m_game->assets ().getAnimation ( animName ).getSize ()), blockMove, blockVis );            
		}

		else if ( firstWord == "NPC" )
		{
            std::vector<Vec2> potrol_Positions;
            iss >> animName >> roomX >> roomY >> posX >> posY >> blockMove >> blockVis>>
                maxHealth>>damage>>behaviourName>>patrolSpeed;
            //  Calculate the entity's center point
            Vec2 calcPosition = getPosition ( roomX, roomY, posX, posY );
            //  Create the new tile entity
            auto e = m_entityManager.addEntity ( "npc" );
            e->addComponent<CAnimation> ( m_game->assets ().getAnimation ( animName ), true );
            e->addComponent<CTransform> (calcPosition);
            e->addComponent<CBoundingBox> ( ( m_game->assets ().getAnimation ( animName ).getSize () ), blockMove, blockVis );
            e->addComponent<CHealth>(maxHealth, maxHealth);
            e->addComponent<CDamage>(damage);
            //  Store the NPC's position coordinates based on behaviour type
            if (behaviourName == "Follow" )
            {
                e->addComponent<CFollowPlayer>(calcPosition, patrolSpeed);
                // Get the next entity
               break;
            }
            //  If the npc patrols get the rest of the patrol coordinates
            if ( behaviourName  == "Patrol" )
            {
                iss>>numPatrolSpots;
                //  Read in the positions of patrol spots
                for (int i = 0;  i < numPatrolSpots; i++)
                {
                    //  Store the patrol positions
                    iss>>patrolPosX>>patrolPosY;
                    Vec2 patrolPos = getPosition ( roomX, roomY, patrolPosX, patrolPosY);
                    potrol_Positions.push_back(patrolPos);
                }
                e->addComponent<CPatrol>(potrol_Positions, patrolSpeed);
            }
		}
		else
		{
			std::cout << "Error in file reading";
		}
	}
    // STUDENT TODO:
    // Load the level file and put all entities in the manager
    // Use the getPosition() function below to convert room-tile coords to game world coords

    spawnPlayer();
}

// This function takes in a room position and a grid position and 
//  Returns a Vec2 indicating where the CENTER position of the Entity is
Vec2 Scene_Zelda::getPosition(int rx, int ry, int tx, int ty) const
{
    //  Convert supplied coordinates to pixels
    float roomX = rx*20*64;
    float roomY = ry*12*64;
    float tileX     = (tx*64)-32 ;
    float tileY = ( ty * 64 ) - 32;
    auto posX = roomX+tileX ;
    auto posY = roomY + tileY;

    return Vec2(posX, posY);
}
                              
void Scene_Zelda::spawnPlayer()
{
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(Vec2(640, 480));
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
    m_player->addComponent<CBoundingBox>(Vec2(48, 48), true, false);
    m_player->addComponent<CHealth>(7, 3);
    //  Add the invincibility component to the player, set to false
    m_player->addComponent<CInvincibility>(0);

    // STUDENT TODO:
    // Implement this function so that it uses the parameters input from the level file
    // Those parameters should be stored in the m_playerConfig variable
}
                              
void Scene_Zelda::spawnSword(std::shared_ptr<Entity> entity)
{
    // Student TODO:
    //
    // Implement the spawning of the sword, which:
    // - should be given the appropriate lifespan
    // - should spawn at the appropriate location based on player's facing direction
    // - be given a damage value of 1
    // - should play the "Slash" sound
}

void Scene_Zelda::update()
{
    //  Check if the game is paused
    if (m_paused)
    {
        //  Game is currently paused, no systems updating
    }
    else
    {
		//  Update all entities
		m_entityManager.update ();
		//  Update all systems
		sAI ();
		sMovement ();
		sStatus ();
		sCollision ();
		sAnimation ();
		sCamera ();

		m_currentFrame++;
    }

}

void Scene_Zelda::sMovement()
{
    //  Store some attributes about the player
	auto& pTransform = m_player->getComponent<CTransform> ();               //  Get a reference to the players Transform attributes
	auto& pInput = m_player->getComponent<CInput> ();                                //  Get a reference to the players Input attributes
	auto& playerV1 = m_player->getComponent<CTransform> ().velocity;    //  Get a reference to the players velocity
	auto    playerV2 = Vec2 ( 0.0f, 0.0f );                                                                //  A new variable to hold the players updated velocity
	auto& state = m_player->getComponent<CState> ().state;                        //  Get a reference to the players State attributes
	

	//  Store previous position before updating
	pTransform.prevPos = pTransform.pos;

	//  Update player movement based on input
	if ( pInput.right )
	{
        //  Update the direction the player is facing
		m_player->getComponent<CTransform> ().facing = Vec2 ( 1.0f, 1.0f );
        //  Update the players animation
		if ( state == "StandRight" ) { state = "RunRight"; }
        //  Update the players velocity
		playerV2.x += m_playerConfig.SPEED;
	}
	if ( pInput.left )
	{
		//  See pInput.right for comments about each lines intention
		m_player->getComponent<CTransform> ().facing = Vec2 ( -1.0f, 1.0f );
		if ( state == "StandRight" ) { state = "RunRIght"; }
		playerV2.x -= m_playerConfig.SPEED;
	}
	if ( pInput.up )
	{
        //  See pInput.right for comments about each lines intention
		m_player->getComponent<CTransform> ().facing = Vec2 ( 0.0f,- 1.0f );
		if ( state == "StandUp" ) { state = "RunUp"; }
		playerV2.y -= m_playerConfig.SPEED;        
	}
	if ( pInput.down )
	{
		//  See pInput.right for comments about each lines intention
		m_player->getComponent<CTransform> ().facing = Vec2 ( 0.0f, 1.0f );
		if ( state == "StandDown " ) { state = "RunDown"; }
		playerV2.y += m_playerConfig.SPEED;
	}

	pTransform.velocity = playerV2;
	pTransform.pos += playerV2;
    /*  TODO: OLD CODE FROM A3 - Constance
	//  Move the bullets on the map
	for ( auto e : m_entityManager.getEntities ( "bullet" ) )
	{
		auto& entityX = e->getComponent<CTransform> ();
		//  Check the direction of travel
		if ( entityX.scale.x > 0 ) {
			//  Bullet is traveling right
			entityX.pos.x += entityX.velocity.x;
		}
		else {
			//  Bullet is traveling left
			entityX.pos.x -= entityX.velocity.x;
		}
	}*/
}

// Set the appropriate attributes to initialize the registered player input
void Scene_Zelda::sDoAction(const Action& action)
{
    //  Get a quick reference to the players state and input attributes
	auto& state = m_player->getComponent<CState> ().state;
	auto& playerInput = m_player->getComponent<CInput> ();

    //  At the start of the action, get the requested action
    if (action.type() == "START")
    {
        //  Game settings
                if (action.name() == "PAUSE")                           { setPaused(!m_paused); }
        else if (action.name() == "QUIT")                             { onEnd(); }
        else if (action.name() == "TOGGLE_FOLLOW")      { m_follow = !m_follow; }
        else if (action.name() == "TOGGLE_TEXTURE")     { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION")  { m_drawCollision = !m_drawCollision; }

		//  Player movement inputs
		//  These actions will not be performed while the game is paused
		if ( !m_paused )
		{
			if ( action.name () == "RIGHT" )
			{
				playerInput.right = true;
			}
			else if ( action.name () == "LEFT" )
			{
				playerInput.left = true;
			}
			else if ( action.name () == "UP" )
			{
				playerInput.up = true;
			}
			else if ( action.name () == "DOWN" )
			{
                playerInput.down = true;
			}
            else if (action.name() == "ATTACK" )
            {
                playerInput.attack = true;
            }
        }
        if ( !m_paused ) {
	        //  The Action has ended
	        if ( action.type () == "END" )
	        {
		        if ( action.name () == "RIGHT" )
		        {
			        playerInput.right = false;
			        if ( state == "RunRight" ) { state = "StandRight"; }
		        }
		        else if ( action.name () == "LEFT" )
		        {
			        playerInput.left = false;
                    if ( state == "RunRight" ) { state = "StandRight"; }
		        }
		        else if ( action.name () == "UP" )
		        {
			        playerInput.up = false;
                    if ( state == "RunUp" ) { state = "StandUp"; }
		        }

		        else if ( action.name () == "DOWN" )
		        {
                    playerInput.down = false;
                    if ( state == "RunDown" ) { state = "StandDown"; }
		        }
				else if ( action.name () == "ATTACK" )
				{
					playerInput.attack = true;
				}
	        }
	    }
    }
}

void Scene_Zelda::sAI()
{
    // STUDENT TODO: Implament Enemy AI
    //
    // Implement Follow behavior
    // Implement Patrol behavior
}

void Scene_Zelda::sStatus()
{
    // STUDENT TODO: 
    // Implement Lifespan Here
    // Implement Invincibility Frames here
}
                              
void Scene_Zelda::sCollision()
{
    // STUDENT TODO:
    //
    // Implement entity - tile collisions
    // Implement player - enemy collisions with appropriate damage calculations
    // Implement Sword - NPC collisions
    // Implement black tile collisions / 'teleporting'
    // Implement entity - heart collisions and life gain logic
    //
    // You may want to use helper functions for these behaviors or this function will get long 
}
                              
void Scene_Zelda::sAnimation()
{
    // STUDENT TODO:
    //
    // Implement player facing direction animation
    // Implement sword animation based on player facing
    //    The sword should move if the player changes direction mid swing
    // Implement destruction of entities with non-repeating finished animations
}

void Scene_Zelda::sCamera()
{
    // STUDENT TODO: 
    //
    // Implement camera view logic
                              
    // get the current view, which we will modify in the if-statement below
    sf::View view = m_game->window().getView();

    if (m_follow)
    {
        // calculate view for player follow camera
    }
    else
    {
        // calcluate view for room-based camera
    }

    // then set the window view
    m_game->window().setView(view);
}
                              
void Scene_Zelda::onEnd()
{
    // STUDENT TODO:
    //
    // Implement scene end
    // - stop the music
    // - play the menu music
    // - change scene to menu
}

void Scene_Zelda::sRender()
{
    // RENDERING DONE FOR YOU

    m_game->window().clear(sf::Color(255, 192, 122));
    sf::RectangleShape tick({ 1.0f, 6.0f });
    tick.setFillColor(sf::Color::Black);
                              
    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        // draw entity animations
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();
            sf::Color c = sf::Color::White;
            if (e->hasComponent<CInvincibility>())
            {
                c = sf::Color(255, 255, 255, 128);
            }
                              
            if (e->hasComponent<CAnimation>())
            {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                animation.getSprite().setColor(c);
                m_game->window().draw(animation.getSprite());
            }
        }

        // draw entity health bars
        for (auto e : m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();
            if (e->hasComponent<CHealth>())
            {
                auto& h = e->getComponent<CHealth>();
                Vec2 size(64, 6);
                sf::RectangleShape rect({ size.x, size.y });
                rect.setPosition(transform.pos.x - 32, transform.pos.y - 48);
                rect.setFillColor(sf::Color(96, 96, 96));
                rect.setOutlineColor(sf::Color::Black);
                rect.setOutlineThickness(2);
                m_game->window().draw(rect);
                              
                float ratio = (float)(h.current) / h.max;
                size.x *= ratio;
                rect.setSize({ size.x, size.y });
                rect.setFillColor(sf::Color(255, 0, 0));
                rect.setOutlineThickness(0);
                m_game->window().draw(rect);

                for (int i = 0; i < h.max; i++)
                {
                    tick.setPosition(rect.getPosition() + sf::Vector2f(i * 64 * 1 / h.max, 0));
                    m_game->window().draw(tick);
                }
            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangleshape
    if (m_drawCollision)
    {
        sf::CircleShape dot(4);
        dot.setFillColor(sf::Color::Black);
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto& box = e->getComponent<CBoundingBox>();
                auto& transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));

                if (box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Black); }
                if (box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::Blue); }
                if (!box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Red); }
                if (!box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::White); }
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }

            if (e->hasComponent<CPatrol>())
            {
                auto& patrol = e->getComponent<CPatrol>().positions;
                for (size_t p = 0; p < patrol.size(); p++)
                {
                    dot.setPosition(patrol[p].x, patrol[p].y);
                    m_game->window().draw(dot);
                }
            }
                              
            if (e->hasComponent<CFollowPlayer>())
            {
                sf::VertexArray lines(sf::LinesStrip, 2);
                lines[0].position.x = e->getComponent<CTransform>().pos.x;
                lines[0].position.y = e->getComponent<CTransform>().pos.y;
                lines[0].color = sf::Color::Black;
                lines[1].position.x = m_player->getComponent<CTransform>().pos.x;
                lines[1].position.y = m_player->getComponent<CTransform>().pos.y;
                lines[1].color = sf::Color::Black;
                m_game->window().draw(lines);
                dot.setPosition(e->getComponent<CFollowPlayer>().home.x, e->getComponent<CFollowPlayer>().home.y);
                m_game->window().draw(dot);
            }
        }
    }
}


// Copyright (C) David Churchill - All Rights Reserved
// COMP4300 - 2021FALL - Assignment 4
// Written by David Churchill (dave.churchill@gmail.com)
// Unauthorized copying of these files are strictly prohibited
// Distributed only for course work at Memorial University
// If you see this file online please contact email above
