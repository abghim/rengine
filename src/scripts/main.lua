init = function() 
	rengine.camera.setpos(2, 4, 6)
	rengine.camera.setrot(math.rad(17), math.rad(-10), math.rad(5))

	rengine.actor.new("assets/star_destroyer.obj")
end

update = function()
end

local step = 0.5
local angle = math.rad(3)
local actor_id = 0

keybind = function(scancode, shift)
	local keys = rengine.sdlkeys

	if scancode == keys.S then
		if shift then rengine.actor.setpos_delta(actor_id, 0, 0, step)
		else rengine.camera.setpos_delta(0, 0, step) end
	elseif scancode == keys.W then
		if shift then rengine.actor.setpos_delta(actor_id, 0, 0, -step)
		else rengine.camera.setpos_delta(0, 0, -step) end
	elseif scancode == keys.Q then
		if shift then rengine.actor.setpos_delta(actor_id, 0, step, 0)
		else rengine.camera.setpos_delta(0, step, 0) end
	elseif scancode == keys.E then
		if shift then rengine.actor.setpos_delta(actor_id, 0, -step, 0)
		else rengine.camera.setpos_delta(0, -step, 0) end
	elseif scancode == keys.A then
		if shift then rengine.actor.setpos_delta(actor_id, step, 0, 0)
		else rengine.camera.setpos_delta(step, 0, 0) end
	elseif scancode == keys.D then
		if shift then rengine.actor.setpos_delta(actor_id, -step, 0, 0)
		else rengine.camera.setpos_delta(-step, 0, 0) end
	elseif scancode == keys.Z then
		rengine.camera.setrot_delta(0, 0, angle)
	elseif scancode == keys.X then
		rengine.camera.setrot_delta(0, 0, -angle)
	elseif scancode == keys.UP then
		rengine.camera.setrot_delta(0, -angle, 0)
	elseif scancode == keys.DOWN then
		rengine.camera.setrot_delta(0, angle, 0)
	elseif scancode == keys.RIGHT then
		rengine.camera.setrot_delta(angle, 0, 0)
	elseif scancode == keys.LEFT then
		rengine.camera.setrot_delta(-angle, 0, 0)
	elseif scancode == keys.H then
		rengine.actor.setrot_delta(actor_id, -angle, 0, 0)
	elseif scancode == keys.L then
		rengine.actor.setrot_delta(actor_id, angle, 0, 0)
	elseif scancode == keys.K then
		rengine.actor.setrot_delta(actor_id, 0, -angle, 0)
	elseif scancode == keys.J then
		rengine.actor.setrot_delta(actor_id, 0, angle, 0)
	elseif scancode == keys.U then
		rengine.actor.setrot_delta(actor_id, 0, 0, -angle)
	elseif scancode == keys.I then
		rengine.actor.setrot_delta(actor_id, 0, 0, angle)
	end
end
