-----
Program Specification
-----

## Assets

- Assets are loaded once at the beginning of the program and stored in the Assets class, which is stored
  by the GameEngine class
- All Assets are defined in assets.txt, see syntax below

## Entities

- All entity positions denote the center of their rectangular sprite.
  It also denotes the center of the bounding box, if it has one.
- This is set via `sprite.setOrigin()` in the Animation class consutrctor

## Player:

- The player has several animations: Stand, Run and Air
- The player moves with the following controls:
  Left: A key, Right: D key, Jump: W key, Shoot: Space Key
- The player can move or shoot at any time during the game. This mean the player can move left/right while in the air
- The player can only jump if it is currently standing on a tile
- If the jump key is held, the player should not continuously jump, but instead it should only jump once per button
  press. If the player lets go of the jump key mid-jump, it should start falling back down immediately
- If the player moves left/right, the player's sprite will face in that direction until the other direction has been
  pressed
- Bullets shot by the player travel in the drection the player is facing
- The player collides with 'Tile' entities in the level (see level syntax) and cannot move through them. The player land
  on a Tile entity and stand in place if it falls on it from above
- The player does not collide with 'Dev' (decoration) entities in the level
- If the player falls below the bottom of the screen, they respawn at the start
- The player should have a Gravity component which constantly accelerates it downward on the screen until it collides
  with a tile
- The player has a maximum speed specified in the level file (see below) which it should not exceed in either x or y
  direction
- The player will be given a CBoundingBox of a size specified in the level file
- The player's sprite and bouding box are centered on the player's position

## Animations:

- See below for Animation asset specification
- Animations are implemented by storing multiple frames inside a texture
- The Animation class handles frame advancement based on animation speed
- Animations can be repeating (loop forever) or non-repeating (play once)
- Any entity with a non-repeating animation be should destroyed once its Animation's hasEnded() returns true (has
  finished one cycle)

## Decoration Entities:

- Decoration entities ('Dec' in a level file) are simply drawn to the screen, and do not interact with any other
  entities in the game in any way
- Decorations can be given any Animation in the game, but intuitively they should be reserved for things like clouds,
  bushes, etc

## Tiles:

- Tiles are Entities that define the level geometry and interact with players
- Tiles can be given any Animation that is defined in the Assets file
- Tiles will be given a CBoundingBox equal to the size of the
  animation `tile->getComponent<CAnimation>().animation.getSize()`
- Tile have different behaviour depending on which Animation they are given
    - Brick Tiles:
        - Brick Tiles are given the 'Brick' Animation
        - When a brick tile collides with a bullet, or is hit by a player from below:
            - Its animation should change to 'Explosion' (non-repeat)
            - Non-repeating animation entities are destroyed when `hasEnded()` is true
            - Its CBoundingBox component should be removed
    - Question Tiles:
        - Question Tiles are given the 'Question' Animation when created
        - When a Question tile is hit by a player from below, 2 things happen:
            - Its Animation changes to the darker 'Question2' animation
            - A temporary lifespan entity with the 'Coin' animation should appear for 30 frames, 64 pixels above the
              location of the Question entity

## Misc:

- Pressing the 'P' key should pause the game
- Pressing the 'T' key toggles drawing textures
- Pressing the 'C' key toggles drawing bounding boxes of entities
- Pressing the 'G' key toggles drawing of the grid
- Pressing the 'ESC' key should go 'back' to the Main Menu, or quit if on the Main Menu

# Assets File Specification

There will be three different line types in the Assets file, each of which correspond to a different type of Asset. They
are as follows:

### Texture Asset Specification:

TEXTURE **N** **P**

| Parameter | Description         | Type        | Example                          |
|-----------|---------------------|-------------|----------------------------------|
| N         | Name of the texture | std::string | MainCharacterTexture             | 
| P         | File path           | std::string | ./resources/textures/texture.jpg | 

### Animation Asset Specification:

ANIMATION **N** **T** **F** **S**

(texture should be loaded before the animation)

| Parameter | Description                               | Type        | Example             |
|-----------|-------------------------------------------|-------------|---------------------|
| N         | Name of the animation                     | std::string | MainCharacterAnim   | 
| T         | Name of the texture                       | std::string | MainChracterTexture | 
| F         | Number of frames in the Animation         | int         | 4                   | 
| S         | Number of game frames between Anim Frames | int         | 10                  | 

### Font Asset Specification:

FONT **N** **P**

| Parameter | Description      | Type        | Example                    |
|-----------|------------------|-------------|----------------------------|
| N         | Name of the font | std::string | MainFont                   |
| P         | File path        | std::string | ./resources/fonts/font.ttf |

# Level Specification File

Game levels will be specified by a Level file, which will contain
a list of entity specifications, one per line.
It will also contain a single line which specifies properties of the player in that level.

### Note

All (GX, GY) positions given in the level specification file are given in
'grid' coordinates. The 'grid' cells are of size 64 x 64 pixels, and the entity
should be position such that the bottom left corner of its texture
is aligned with the bottom left corner of the given grid coordinate.
The grid starts at (0, 0) in the bottom-left of the screen, and can be seen by
pressing the 'G' key while the game is running.

### Tile Entity Specification:

Tile **N** **GX** **GY**

| Parameter | Description    | Type        | Example   |
|-----------|----------------|-------------|-----------|
| N         | Animation Name | std::string | Explosion |
| GX        | Grid X         | int         | 3         |
| GY        | Grid Y         | int         | 5         |

### Decoration Entity Specification:

Dec **N** **GX** **GY**

| Parameter | Description    | Type        | Example   |
|-----------|----------------|-------------|-----------|
| N         | Animation Name | std::string | Explosion |
| GX        | Grid X         | int         | 3         |
| GY        | Grid Y         | int         | 5         |

### Player Specification:

Player **GX** **GY** **CW** **CH** **SX** **SY** **SM** **GY** **B**

| Parameter | Description      | Type        | Example    |
|-----------|------------------|-------------|------------|
| GX        | Starting Grid X  | int         | 3          |
| GY        | Starting Grid Y  | int         | 5          |
| CW        | Width of BBox    | float       | 64         |
| CH        | Height of BBox   | float       | 64         |
| SX        | Left/Right Speed | float       | 32         |
| SY        | Jump Speed       | float       | 5          |
| SM        | Max Speed        | float       | 10         |
| GY        | Gravity Force    | float       | 9.8        |
| B         | Bullet Animation | std::string | BulletAnim |
