-- install SWIG and LUA-DEV packages in your environment platform
-- exec "make lua" from v4p source top directory;
-- then come in this directory and execute "lua demov4p.lua"
require("v4p");
v4p.v4pi_init(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);
v4p.v4p_init();
v4p.v4p_setBGColor(v4p.blue);
 
pSprite = v4p.v4p_new(v4p.absolute, v4p.red, 10);

v4p.v4p_rect(pSprite, -v4p.v4pDisplayWidth / 3 + v4p.v4pDisplayWidth, -v4p.v4pDisplayHeight / 3, v4p.v4pDisplayWidth / 3, v4p.v4pDisplayHeight / 3);

pClone = v4p.v4p_addClone(pSprite);

i = 128;
v4p.v4p_setView(-v4p.v4pDisplayWidth * i / 256, -v4p.v4pDisplayHeight * i / 256, v4p.v4pDisplayWidth + v4p.v4pDisplayWidth * i / 256, v4p.v4pDisplayHeight + v4p.v4pDisplayHeight * i / 256);
v4p.v4p_transformClone(pSprite, pClone, v4p.v4pDisplayWidth / 2, v4p.v4pDisplayWidth / 2, 0, 0);
v4p.v4p_render();
io.stdin:read'*l';
v4p.v4pi_destroy();
v4p.v4p_quit();
