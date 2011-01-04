-- install SWIG and LUA-DEV packages in your environment platform
-- exec "make lua" from v4p source top directory;
-- then come in this directory and execute "lua demov4p.lua"
require("v4p");
v4p.v4pDisplayInit(1, 0);
v4p.v4pInit();
v4p.v4pSetBGColor(v4p.blue);
 
pSprite = v4p.v4pPolygonNew(v4p.absolute, v4p.red, 10);

v4p.v4pPolygonRect(pSprite, -v4p.v4pDisplayWidth / 3 + v4p.v4pDisplayWidth, -v4p.v4pDisplayHeight / 3, v4p.v4pDisplayWidth / 3, v4p.v4pDisplayHeight / 3);

pClone = v4p.v4pAddClone(pSprite);

i = 128;
v4p.v4pSetView(-v4p.v4pDisplayWidth * i / 256, -v4p.v4pDisplayHeight * i / 256, v4p.v4pDisplayWidth + v4p.v4pDisplayWidth * i / 256, v4p.v4pDisplayHeight + v4p.v4pDisplayHeight * i / 256);
v4p.v4pPolygonTransformClone(pSprite, pClone, v4p.v4pDisplayWidth / 2, v4p.v4pDisplayWidth / 2, 0, 0);
v4p.v4pRender();
io.stdin:read'*l';
v4p.v4pDisplayQuit();
v4p.v4pQuit();
