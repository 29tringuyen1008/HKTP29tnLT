local Assets = GetAssets()
local sound_buffer = GetChild(Assets, "camera_shutter")

local sound_name = "camera_shutter_sound"
local is_found, existing_sound = FindFirstChild(Assets, sound_name)
if existing_sound == nil then
	existing_sound = CreateObject("Sound")
	ModifyObj(existing_sound, {
		Name = sound_name,
		Parent = Assets
	})
	SetAudioBuffer(existing_sound, sound_buffer)
	PlaySound(existing_sound)
else
	PlaySound(existing_sound)
end