local destroyer_id = 0
local tie_a_id = 1
local tie_b_id = 2
local elapsed = 0.0

local shot_length = 32.0
local hold_length = 2.5

local function lerp(a, b, t)
	return a + (b - a) * t
end

local function clamp(v, lo, hi)
	if v < lo then return lo end
	if v > hi then return hi end
	return v
end

local function smoothstep(t)
	t = clamp(t, 0.0, 1.0)
	return t * t * (3.0 - 2.0 * t)
end

local function atan2(y, x)
	return math.atan(y, x)
end

local function v3(x, y, z)
	return { x = x, y = y, z = z }
end

local function vadd(a, b)
	return v3(a.x + b.x, a.y + b.y, a.z + b.z)
end

local function vsub(a, b)
	return v3(a.x - b.x, a.y - b.y, a.z - b.z)
end

local function vscale(a, s)
	return v3(a.x * s, a.y * s, a.z * s)
end

local function vlen(a)
	return math.sqrt(a.x * a.x + a.y * a.y + a.z * a.z)
end

local function vnorm(a)
	local n = vlen(a)
	if n < 1e-6 then return v3(0.0, 0.0, 1.0) end
	return v3(a.x / n, a.y / n, a.z / n)
end

local function cross(a, b)
	return v3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	)
end

local function basis_from_camera(yaw, pitch)
	local cp = math.cos(pitch)
	local forward = vnorm(v3(math.sin(yaw) * cp, -math.sin(pitch), math.cos(yaw) * cp))
	local right = vnorm(cross(v3(0.0, 1.0, 0.0), forward))
	local up = vnorm(cross(forward, right))
	return right, up, forward

end

local function bezier(a, b, c, t)
	local ab = v3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t))
	local bc = v3(lerp(b.x, c.x, t), lerp(b.y, c.y, t), lerp(b.z, c.z, t))
	return v3(lerp(ab.x, bc.x, t), lerp(ab.y, bc.y, t), lerp(ab.z, bc.z, t))
end

local function reset_shot()
	elapsed = 0.0
	print("Imperial Flyover: press R to replay.")
end

local function camera_pose(phase)
	local rise_phase = smoothstep(smoothstep(clamp((phase - 0.95) / 0.05, 0.0, 1.0)))
	local push_phase = smoothstep(clamp((phase - 0.97) / 0.03, 0.0, 1.0))
	local px = lerp(0.45, -0.30, phase)
	local py = lerp(-4.6, -2.8, phase) + lerp(0.0, 62.0, rise_phase)
	local pz = lerp(16.2, 4.0, phase) + lerp(0.0, -10.0, rise_phase) + lerp(0.0, -18.0, push_phase)
	local yaw = lerp(math.rad(0.7), math.rad(-1.6), phase)
	local pitch = lerp(math.rad(-7.5), math.rad(-21.0), phase) + lerp(0.0, math.rad(-48.0), rise_phase) + lerp(0.0, math.rad(-4.0), push_phase)
	local roll = lerp(math.rad(1.0), math.rad(-1.0), phase) + lerp(0.0, math.rad(-3.0), rise_phase)
	return px, py, pz, yaw, pitch, roll
end

local function destroyer_pose(phase)
	local settle_phase = smoothstep(clamp((phase - 0.82) / 0.18, 0.0, 1.0))
	local x = lerp(1.4, -0.8, phase)
	local y = lerp(24.0, 8.2, phase) + lerp(0.0, -1.9, settle_phase)
	local z = lerp(84.0, -92.0, phase) + lerp(0.0, -8.0, settle_phase)

	-- The mesh's long axis is along Z; yaw pi keeps the ship aligned correctly for the flyover.
	local yaw = math.pi
	local pitch = math.rad(2.2)
	local roll = math.rad(0.8)
	return x, y, z, yaw, pitch, roll
end

local function set_tie_hidden(id)
	rengine.actor.setpos(id, 0.0, -2000.0, 0.0)
	rengine.actor.setrot(id, 0.0, 0.0, 0.0)
end

local function update_tie(id, lane, tie_phase, cam_pos, right, up, forward, destroyer_pos)
	local start = vadd(vadd(vadd(cam_pos, vscale(forward, 42.0)), vscale(right, lane * 9.0)), vscale(up, -2.0))
	local mid = vadd(vadd(vadd(cam_pos, vscale(forward, -22.0)), vscale(right, lane * 16.0)), vscale(up, 4.0))
	local finish = vadd(vadd(vadd(destroyer_pos, vscale(forward, -150.0)), vscale(right, lane * 26.0)), vscale(up, 10.0))

	local pos = bezier(start, mid, finish, tie_phase)
	local look = bezier(start, mid, finish, clamp(tie_phase + 0.02, 0.0, 1.0))
	local dir = vnorm(vsub(look, pos))
	local horiz = math.sqrt(dir.x * dir.x + dir.z * dir.z)
	local yaw = atan2(dir.x, dir.z)
	local pitch = atan2(-dir.y, horiz)
	local roll = lane * math.rad(-20.0) * (1.0 - tie_phase)

	rengine.actor.setpos(id, pos.x, pos.y, pos.z)
	rengine.actor.setrot(id, yaw, pitch, roll)
end

local function update_ties(phase, cx, cy, cz, cyaw, cpitch, sx, sy, sz)
	local tie_phase = smoothstep(clamp((phase - 0.82) / 0.16, 0.0, 1.0))
	if tie_phase <= 0.0 then
		set_tie_hidden(tie_a_id)
		set_tie_hidden(tie_b_id)
		return
	end

	local cam_pos = v3(cx, cy, cz)
	local destroyer_pos = v3(sx, sy, sz)
	local right, up, forward = basis_from_camera(cyaw, cpitch)

	update_tie(tie_a_id, -1.0, tie_phase, cam_pos, right, up, forward, destroyer_pos)
	update_tie(tie_b_id, 1.0, tie_phase, cam_pos, right, up, forward, destroyer_pos)
end

init = function()
	rengine.actor.new("assets/star_destroyer.obj")
	rengine.actor.new("assets/tie.obj")
	rengine.actor.new("assets/tie.obj")
	set_tie_hidden(tie_a_id)
	set_tie_hidden(tie_b_id)
	reset_shot()
end

update = function(delta_t)
	elapsed = elapsed + delta_t
	local cycle = shot_length + hold_length
	if elapsed > cycle then
		elapsed = 0.0
	end

	local phase = smoothstep(clamp(elapsed / shot_length, 0.0, 1.0))

	local cx, cy, cz, cyaw, cpitch, croll = camera_pose(phase)
	rengine.camera.setpos(cx, cy, cz)
	rengine.camera.setrot(cyaw, cpitch, croll)

	local sx, sy, sz, syaw, spitch, sroll = destroyer_pose(phase)
	rengine.actor.setpos(destroyer_id, sx, sy, sz)
	rengine.actor.setrot(destroyer_id, syaw, spitch, sroll)
	update_ties(phase, cx, cy, cz, cyaw, cpitch, sx, sy, sz)
end

keybind = function(scancode, shift)
	local keys = rengine.sdlkeys
	if scancode == keys.R then
		reset_shot()
	end
end
