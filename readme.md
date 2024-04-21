A simple platform game demo/engine <br>
Written entirely in C++ and SFML <br>

>
>Features I've implemented so far:
>- Classic platform game style player movement, with coyote jump and other quality of life changes added
>- Pixel perfect terrain collision using AABB
>- System for key frame animations (with support for interpolation)
>- Animated sprites using the mentioned system
>- A simple .xml parser
>- A simple .tmx parser, which allows the game to directly load level data created with *[Tiled level editor](https://www.mapeditor.org)*
>- System for pretty and precise camera movement with old school screen transitions
>- Pixel perfect bitmap font parser and renderer
>- Very flexible nine slice texture parser and renderer
>

---

>
>Project TODO list:
>- [x] visible terrain tiles
>- [ ] enemies, goombalike
>- [ ] simple collectables
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