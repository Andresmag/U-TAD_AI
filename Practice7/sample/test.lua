function createImage(texture_name, char_size)
	local gfxQuad = MOAIGfxQuad2D.new()
	gfxQuad:setTexture(texture_name)
	char_size = 64
	gfxQuad:setRect(-char_size/2, -char_size/2, char_size/2, char_size/2)
	gfxQuad:setUVRect(0, 0, 1, 1)
	return gfxQuad
end

MOAISim.openWindow("game", 1024, 768)

viewport = MOAIViewport.new()
viewport:setSize (1024, 768)
viewport:setScale (1024, -768)

layer = MOAILayer2D.new()
layer:setViewport(viewport)
MOAISim.pushRenderPass(layer)

char_size = 64
gfxQuadIdle = createImage("dragonIdle.png", char_size)
gfxQuadWindup = createImage("dragonWindup.png", char_size)
gfxQuadAlarm = createImage("dragonAlarm.png", char_size)
gfxQuadAttack = createImage("dragonAttack.png", char_size)
gfxQuadHit = createImage("dragonHit.png", char_size)
gfxQuadDead = createImage("dragonDead.png", char_size)

prop = MOAIProp2D.new()
prop:setDeck(gfxQuadIdle)
entity  = Character.new()
enemy = Character.new()
entity:setEnemy(enemy)

-- Add prop to be the renderable for this character
entity:addImage(gfxQuadIdle)    -- SetImage(0)  dragonIdle.png
entity:addImage(gfxQuadWindup)  -- SetImage(1)  dragonWindup.png
entity:addImage(gfxQuadAlarm)   -- SetImage(2)  dragonAlarm.png
entity:addImage(gfxQuadAttack)  -- SetImage(3)  dragonAttack.png
entity:addImage(gfxQuadHit)     -- SetImage(4)  dragonHit.png
entity:addImage(gfxQuadDead)    -- SetImage(5)  dragonDead.png

entity:setProp(prop, layer)

-- Start the caracter (allow calls to OnUpdate)
entity:loadBehaviorTree("hunterBT.xml")
entity:start()

entity:setLoc(-200, -200)
entity:setRot(0)
entity:setLinearVel(20, 20)
entity:setAngularVel(30)

-- Add enemy entity
enemy:addImage(gfxQuadIdle)		-- SetImage(0)  dragonIdle.png
enemy:addImage(gfxQuadWindup)	-- SetImage(1)  dragonWindup.png
enemy:addImage(gfxQuadHit)		-- SetImage(2)  dragonHit.png
enemy:addImage(gfxQuadDead)		-- SetImage(3)  dragonDead.png

enemyProp = MOAIProp2D.new()
enemyProp:setDeck(gfxQuadIdle)
enemy:setProp(enemyProp, layer)

enemy:loadBehaviorTree("preyBT.xml")
enemy:start()
enemy:setLoc(200, 200)
enemy:setRot(0)
enemy:setLinearVel(20, 20)
enemy:setAngularVel(30)

-- Enable Debug Draw
debug = MOAIDrawDebug.get();
layer:setDrawDebug(debug)
-- Add this character to draw debug
MOAIDrawDebug.insertEntity(entity)
MOAIDrawDebug.insertEntity(enemy)
mouseX = 0
mouseY = 0

function onClick(down)
  --entity:setLoc(mouseX, mouseY)
  -- entity:setRot(0)
  enemy:setLoc(mouseX, mouseY)
end

function pointerCallback(x, y)
    mouseX, mouseY = layer:wndToWorld(x, y)
end

MOAIInputMgr.device.mouseLeft:setCallback(onClick)
MOAIInputMgr.device.pointer:setCallback(pointerCallback)