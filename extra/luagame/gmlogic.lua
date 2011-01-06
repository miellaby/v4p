-- The scare demo as a lua script
-- Loaded and run by luagame.c
STRESS_AMOUNT = 32
iu = 0
diu = STRESS_AMOUNT
liu  = 3
angle = 0
pCloneMatrix={}


function gmOnInit()
  v4p.v4pDisplayInit(1, 0)
  v4p.v4pInit()
  v4p.v4pSetBGColor(v4p.blue)
 
  pSprite=v4p.v4pPolygonNew(v4p.absolute, v4p.red, 10)
  v4p.v4pPolygonRect(pSprite, -v4p.v4pDisplayWidth / 3 + v4p.v4pDisplayWidth, -v4p.v4pDisplayHeight / 3, v4p.v4pDisplayWidth / 3, v4p.v4pDisplayHeight / 3)

  for j= 0,STRESS_AMOUNT-1 do
    for k = 0,STRESS_AMOUNT-1 do
      pCloneMatrix[1+j*STRESS_AMOUNT+k] = v4p.v4pAddClone(pSprite)
    end
  end
  return false
end

function gmOnIterate()
  i = iu
  if diu>0 and i>128 * STRESS_AMOUNT then diu=-diu end
  if diu<0 and i + diu < -100 then
     diu=-diu
     liu=liu-1
  end
  v4p.v4pSetView(-v4p.v4pDisplayWidth * i / 256, -v4p.v4pDisplayHeight * i / 256, v4p.v4pDisplayWidth + v4p.v4pDisplayWidth * i / 256, v4p.v4pDisplayHeight + v4p.v4pDisplayHeight * i / 256)
  
  local idx=1
  local s=STRESS_AMOUNT-1
  if liu % 4 >= 2 then
      for j=0,s do
        for k=0,s do
          v4p.v4pPolygonTransformClone(pSprite, pCloneMatrix[idx], v4p.v4pDisplayWidth * (1 + k - STRESS_AMOUNT/2) / 2, v4p.v4pDisplayWidth * (1 + j - STRESS_AMOUNT/2)/2, (j * k) + angle / 16, 0)
          idx=idx+1
        end
      end
  end
  iu = iu + diu
  angle = angle + STRESS_AMOUNT
  return (liu < 0)
end
 
function gmOnFrame()
   v4p.v4pRender()
   return false
end

function gmOnQuit()
  v4p.v4pDisplayQuit()
  v4p.v4pQuit()
end

if gmMain then
   gmMain()
else
   require("v4p")
   gmOnInit()
   gmOnIterate()
   gmOnFrame()
   gmOnQuit()
end
