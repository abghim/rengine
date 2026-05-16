local actor_ids = {}
local actors = {}

local lane_x = { -2.4, 0.0, 2.4 }
local lane_y = { -1.6, 0.0, 1.6 }

local player_lane_x = 2
local player_lane_y = 2
local player_z = 7.5

local teapot_count = 3
local speed = 4.4
local spawn_front_z = -18.0
local spawn_gap = 11.0
local spin_speed = math.rad(95)

local score = 0
local best = 0
local game_over_time = 0.0

local function lane_count()
	return #lane_x * #lane_y
end

local function lane_from_index(index)
	local x_index = ((index - 1) % #lane_x) + 1
	local y_index = math.floor((index - 1) / #lane_x) + 1
	return x_index, y_index
end

local function shuffled_lane_indices()
	local values = {}
	for i = 1, lane_count() do
		values[i] = i
	end
	for i = #values, 2, -1 do
		local j = math.random(i)
		values[i], values[j] = values[j], values[i]
	end
	return values
end

local function apply_camera()
	rengine.camera.setpos(lane_x[player_lane_x], lane_y[player_lane_y], player_z)
	rengine.camera.setrot(0.0, 0.0, 0.0)
end

local function place_actor(i)
	local a = actors[i]
	rengine.actor.setpos(actor_ids[i], lane_x[a.lane_x], lane_y[a.lane_y], a.z)
	rengine.actor.setrot(actor_ids[i], a.rx, a.ry, a.rz)
end

local function respawn_actor(i, lane_index, spawn_z)
	local a = actors[i]
	a.lane_x, a.lane_y = lane_from_index(lane_index)
	a.z = spawn_z
	a.rx = math.random() * math.pi * 2
	a.ry = math.random() * math.pi * 2
	a.rz = math.random() * math.pi * 2
	place_actor(i)
end

local function reseed_wave(start_z)
	local lanes = shuffled_lane_indices()
	for i = 1, #actors do
		respawn_actor(i, lanes[i], start_z - (i - 1) * spawn_gap)
	end
end

local function reset_round()
	reseed_wave(spawn_front_z)
	score = 0
	game_over_time = 0.0
	player_lane_x = 2
	player_lane_y = 2
	player_z = 7.5
	apply_camera()
	print("Teapot Panic: dodge the flying teapots with WASD. Q/E moves depth. R resets.")
end

init = function()
	math.randomseed(os.time())

	apply_camera()

	for i = 1, teapot_count do
		rengine.actor.new("assets/utah_teapot.obj")
		actor_ids[i] = i - 1
		actors[i] = {
			lane_x = 2,
			lane_y = 2,
			z = spawn_front_z,
			rx = 0.0,
			ry = 0.0,
			rz = 0.0,
		}
	end

	reset_round()
end

update = function(delta_t)
	if game_over_time > 0.0 then
		game_over_time = game_over_time - delta_t
		if game_over_time <= 0.0 then
			reset_round()
		end
		return
	end

	for i = 1, #actors do
		local a = actors[i]
		a.z = a.z + speed * delta_t
		a.rx = a.rx + spin_speed * 0.35 * delta_t
		a.ry = a.ry + spin_speed * delta_t
		a.rz = a.rz + spin_speed * 0.55 * delta_t

		if math.abs(a.z - player_z) < 0.9 and a.lane_x == player_lane_x and a.lane_y == player_lane_y then
			best = math.max(best, score)
			print(string.format("Crashed! score=%d best=%d", score, best))
			game_over_time = 1.2
			return
		end

		if a.z > player_z + 1.5 then
			score = score + 1
			if score > best then best = score end
			if score % 5 == 0 then
				print(string.format("Score %d  Best %d", score, best))
			end
			local farthest_z = spawn_front_z
			for j = 1, #actors do
				if actors[j].z < farthest_z then
					farthest_z = actors[j].z
				end
			end
			local lanes = shuffled_lane_indices()
			respawn_actor(i, lanes[1], farthest_z - spawn_gap)
		else
			place_actor(i)
		end
	end
end

keybind = function(scancode, shift)
	local keys = rengine.sdlkeys

	if scancode == keys.A then
		player_lane_x = math.max(1, player_lane_x - 1)
		apply_camera()
	elseif scancode == keys.D then
		player_lane_x = math.min(#lane_x, player_lane_x + 1)
		apply_camera()
	elseif scancode == keys.W then
		player_lane_y = math.min(#lane_y, player_lane_y + 1)
		apply_camera()
	elseif scancode == keys.S then
		player_lane_y = math.max(1, player_lane_y - 1)
		apply_camera()
	elseif scancode == keys.Q then
		player_z = player_z - 0.4
		apply_camera()
	elseif scancode == keys.E then
		player_z = player_z + 0.4
		apply_camera()
	elseif scancode == keys.R then
		reset_round()
	end
end
