local cam = {
	x = 0.0,
	y = 0.2,
	z = 8.0,
	yaw = 0.0,
	pitch = 0.0,
	roll = 0.0,
	speed = 8.0,
}

local function v3(x, y, z)
	return { x = x, y = y, z = z }
end

local keys = nil
local targets = {}

local throttle_step = 1.2
local turn_step = math.rad(3.0)
local max_speed = 18.0
local min_speed = 3.0
local distance_score = 0.0
local best_score = 0.0
local crash_timer = 0.0

local obstacle_count = 1
local obstacle_base_z = -55.0
local obstacle_gap = 18.0

local function clamp(v, lo, hi)
	if v < lo then return lo end
	if v > hi then return hi end
	return v
end

local function basis(rot)
	local cy = math.cos(rot.yaw)
	local sy = math.sin(rot.yaw)
	local cp = math.cos(rot.pitch)
	local sp = math.sin(rot.pitch)
	local cr = math.cos(rot.roll)
	local sr = math.sin(rot.roll)

	local right = {
		x = cy * cr + sy * sp * sr,
		y = cp * sr,
		z = -sy * cr + cy * sp * sr,
	}

	local up = {
		x = -cy * sr + sy * sp * cr,
		y = cp * cr,
		z = sy * sr + cy * sp * cr,
	}

	local forward = {
		x = sy * cp,
		y = -sp,
		z = cy * cp,
	}

	return right, up, forward
end

local function gameplay_forward(rot)
	local _, _, forward = basis(rot)
	return v3(-forward.x, -forward.y, -forward.z)
end

local function set_camera()
	rengine.camera.setpos(cam.x, cam.y, cam.z)
	rengine.camera.setrot(cam.yaw, cam.pitch, cam.roll)
end

local function world_from_local(base, right, up, forward, lx, ly, lz)
	return {
		x = base.x + right.x * lx + up.x * ly + forward.x * lz,
		y = base.y + right.y * lx + up.y * ly + forward.y * lz,
		z = base.z + right.z * lx + up.z * ly + forward.z * lz,
	}
end

local function set_actor_pose(id, pos, rot)
	rengine.actor.setpos(id, pos.x, pos.y, pos.z)
	rengine.actor.setrot(id, rot.x, rot.y, rot.z)
end

local function reset_target(i, zpos)
	targets[i] = {
		x = (math.random() * 2.0 - 1.0) * 9.0,
		y = (math.random() * 2.0 - 1.0) * 5.0,
		z = zpos,
		rx = 0.0,
		ry = math.pi,
		rz = 0.0,
	}
end

local function reset_round()
	cam.x = 0.0
	cam.y = 0.2
	cam.z = 8.0
	cam.yaw = 0.0
	cam.pitch = 0.0
	cam.roll = 0.0
	cam.speed = 8.0
	distance_score = 0.0
	crash_timer = 0.0
	set_camera()

	for i = 1, obstacle_count do
		reset_target(i, obstacle_base_z - (i - 1) * obstacle_gap)
	end

	print("Star Run: W/S pitch, A/D yaw, Shift+A/D roll, Shift+W/S throttle. Dodge the TIE.")
end

init = function()
	math.randomseed(os.time())
	keys = rengine.sdlkeys

	for i = 1, obstacle_count do
		rengine.actor.new("assets/tie.obj")
	end

	reset_round()
end

update = function(delta_t)
	if crash_timer > 0.0 then
		crash_timer = crash_timer - delta_t
		if crash_timer <= 0.0 then
			reset_round()
		end
		return
	end

	local right, up, basis_forward = basis(cam)
	local forward = gameplay_forward(cam)
	cam.x = cam.x + forward.x * cam.speed * delta_t
	cam.y = cam.y + forward.y * cam.speed * delta_t
	cam.z = cam.z + forward.z * cam.speed * delta_t
	set_camera()
	distance_score = distance_score + cam.speed * delta_t

	local recycle_z = obstacle_base_z
	for i = 1, obstacle_count do
		local rock = targets[i]

		local actor_id = i - 1
		set_actor_pose(actor_id, rock, { x = rock.rx, y = rock.ry, z = rock.rz })

		local dx = rock.x - cam.x
		local dy = rock.y - cam.y
		local dz = rock.z - cam.z
		local dist2 = dx * dx + dy * dy + dz * dz
		if dist2 < 7.5 then
			if distance_score > best_score then best_score = distance_score end
			print(string.format("Hull breach! distance=%.1f best=%.1f", distance_score, best_score))
			crash_timer = 1.4
			return
		end

		if rock.z > cam.z + 6.0 then
			if rock.z < recycle_z then recycle_z = rock.z end
			reset_target(i, cam.z - obstacle_gap * obstacle_count - math.random() * 14.0)
		end
	end
end

keybind = function(scancode, shift)
	if scancode == keys.A then
		if shift then cam.roll = cam.roll - turn_step
		else cam.yaw = cam.yaw - turn_step end
	elseif scancode == keys.D then
		if shift then cam.roll = cam.roll + turn_step
		else cam.yaw = cam.yaw + turn_step end
	elseif scancode == keys.W then
		if shift then cam.speed = clamp(cam.speed + throttle_step, min_speed, max_speed)
		else cam.pitch = clamp(cam.pitch - turn_step, math.rad(-65.0), math.rad(65.0)) end
	elseif scancode == keys.S then
		if shift then cam.speed = clamp(cam.speed - throttle_step, min_speed, max_speed)
		else cam.pitch = clamp(cam.pitch + turn_step, math.rad(-65.0), math.rad(65.0)) end
	elseif scancode == keys.R then
		reset_round()
	end
end
