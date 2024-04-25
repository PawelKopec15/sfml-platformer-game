A simple platform game demo/engine <br>
Written entirely in C++ and SFML <br><br>
Developed with VS Code, with use of *[this handy cmake template](https://github.com/SFML/cmake-sfml-project)*. <br>

![GIF footage of gameplay](media/main_gameplay.gif)
All graphical assets have been created by me, with the exception of [the font](https://zichy.itch.io/kubasta). <br>
When framerate limiter is disabled, the game runs at about 1000FPS on average.

>
>Features I've implemented so far:
>- Classic platform game style player movement, with coyote jump and other quality of life changes added
>- Pixel perfect terrain collision using AABB
>- System for key frame animations (with support for linear interpolation between values)
>- Animated sprites using the mentioned system, with possible operations such as changing texture rect, scaling, changing origin, offset ect.
>- A simple .xml parser
>- A simple .tmx parser, which allows the game to directly load level data created with *[Tiled level editor](https://www.mapeditor.org)*
>- System for pretty and precise camera movement with old school screen transitions, with use of the key frame animation system and tmx parser
>- Pixel perfect bitmap font parser and renderer
>- ChunkMap data structure for chunking terrain and optimizations.
>- Very basic collectable and inventory system


| Game controls : |||
|-----------------------|---|-------------------|
| Left and Right arrows | - | movement          |
| Left Shift            | - | dash              |
| Z                     | - | jump              |
| Numpad 0              | - | toggle debug mode |

<br>

![GIF footage of debug mode](media/debug_gameplay.gif)

| When in debug mode : |||
|-----------------------|---|-------------------|
| Numpad 5 | - | set framerate limiter to 144FPS (default)  |
| Numpad 7 | - | set framerate limiter to 30FPS             |
| Numpad 8 | - | set framerate limiter to 60FPS             |
| Numpad 9 | - | disable framerate limiter (watch Your graphics card!)            |
| Numpad 0              | - | toggle debug mode |

**Also when in debug mode:** <br>
- Delta (time passed in microseconds) and FPS for current frame will be shown <br>
- All colliders and interesting boxes will be visible <br>
- CollisionBodies the player currently interacts with will be highlighted

<br>


---

>
>Project TODO list:
>- [x] visible terrain tiles
>- [X] simple collectables
>- [X] inventory hud
>- [ ] enemies, goombalike
>- [ ] proper chunking when collision
>- [ ] loading levels
>- [ ] proper program structure
>- [ ] source files arrangement
>- [ ] slicing delta if too high
>
>Non essential TODO:
>- [ ] adding terrain objects
>- [ ] bouncy objects
>- [ ] slopes
>- [ ] global variables singleton
>- [ ] one way collisions
>- [ ] tileset texture files being deduced from .tmx file
>