A simple platform game demo/engine <br>
Written entirely in C++ and SFML <br><br>
Developed with VS Code, with use of *[this handy cmake template](https://github.com/SFML/cmake-sfml-project)*.

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

---

>
>Project TODO list:
>- [x] visible terrain tiles
>- [ ] simple collectables
>- [ ] inventory hud
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