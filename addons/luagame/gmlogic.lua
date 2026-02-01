-- The scare demo as a lua script
-- Loaded and run by luagame.c
STRESS_AMOUNT = 32
iu = 0
diu = STRESS_AMOUNT
liu  = 3
angle = 0
pCloneMatrix={}


function g4pOnInit()
  v4p.v4pi_init(1, 0)
  v4p.v4p_init()
  v4p.v4pi_setBgColor(v4p.blue)
 
  pSprite=v4p.v4p_new(v4p.absolute, v4p.red, 10)
  v4p.v4p_rect(pSprite, -v4p.v4p_displayWidth / 3 + v4p.v4p_displayWidth, -v4p.v4p_displayHeight / 3, v4p.v4p_displayWidth / 3, v4p.v4p_displayHeight / 3)

  for j= 0,STRESS_AMOUNT-1 do
    for k = 0,STRESS_AMOUNT-1 do
      pCloneMatrix[1+j*STRESS_AMOUNT+k] = v4p.v4p_add_clone(pSprite)
    end
  end
  return false
end

function g4pOnTick(deltaTime)
  i = iu
  
  -- Use deltaTime for time-based animation
  local timeFactor = deltaTime / 16.0  -- Normalize to ~60fps equivalent
  
  if diu>0 and i>128 * STRESS_AMOUNT then diu=-diu end
  if diu<0 and i + diu < -100 then
     diu=-diu
     liu=liu-1
  end
  v4p.v4p_setView(-v4p.v4p_displayWidth * i / 256, -v4p.v4p_displayHeight * i / 256, v4p.v4p_displayWidth + v4p.v4p_displayWidth * i / 256, v4p.v4p_displayHeight + v4p.v4p_displayHeight * i / 256)
  
  local idx=1
  local s=STRESS_AMOUNT-1
  if liu % 4 >= 2 then
      for j=0,s do
        for k=0,s do
          v4p.v4p_transformClone(pSprite, pCloneMatrix[idx], v4p.v4p_displayWidth * (1 + k - STRESS_AMOUNT/2) / 2, v4p.v4p_displayWidth * (1 + j - STRESS_AMOUNT/2)/2, (j * k) + angle / 16, 0)
          idx=idx+1
        end
      end
  end
  
  -- Use time-based movement
  iu = iu + diu * timeFactor
  angle = angle + STRESS_AMOUNT * timeFactor
  return (liu < 0)
end
 
function g4pOnFrame()
   v4p.v4p_render()
   return false
end

function g4pOnQuit()
  v4p.v4pi_destroy()
  v4p.v4pQuit()
end

if g4pMain then
   g4pMain()
else
   require("v4p")
   g4pOnInit()
   g4pOnIterate()
   g4pOnFrame()
   g4pOnQuit()
end
