#pragma once
#ifndef include_DATA
#define include_DATA


static const int SCREEN_WIDTH = 1280;
static const int SCREEN_HEIGHT = 720;

static const char* REQUIRED_ENTITY_ATTRIBUTES	= "Name";
static const char* OPTIONAL_ENTITY_ATTRIBUTES	= "";
static const char* REQUIRED_ENTITY_CHILDREN		= "Appearance";
static const char* OPTIONAL_ENTITY_CHILDREN		= "Trigger";

static const char* REQUIRED_ACTOR_ATTRIBUTES	= "Name, MoveSpeed, Hitpoints";
static const char* OPTIONAL_ACTOR_ATTRIBUTES	= "Movespeed";
static const char* REQUIRED_ACTOR_CHILDREN		= "Appearance";
static const char* OPTIONAL_ACTOR_CHILDREN		= "Trigger";

static const char* REQUIRED_ITEM_ATTRIBUTES		= "Name";
static const char* OPTIONAL_ITEM_ATTRIBUTES		= "";
static const char* REQUIRED_ITEM_CHILDREN		= "Appearance";
static const char* OPTIONAL_ITEM_CHILDREN		= "Trigger";

static const char* REQUIRED_TILE_ATTRIBUTES		= "Name, IsSolid";
static const char* OPTIONAL_TILE_ATTRIBUTES		= "";
static const char* REQUIRED_TILE_CHILDREN		= "Appearance";
static const char* OPTIONAL_TILE_CHILDREN		= "Trigger";

static const char* REQUIRED_LEVEL_ATTRIBUTES	= "Name, Dimensions, Default";
static const char* OPTIONAL_LEVEL_ATTRIBUTES	= "";
static const char* REQUIRED_LEVEL_CHILDREN		= "PlayerStart";
static const char* OPTIONAL_LEVEL_CHILDREN		= "Blob, Line, EnemySpawn";

#endif