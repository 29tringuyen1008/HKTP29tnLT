--TỆP LUA NÀY CÓ NHỮNG BIẾN VIẾT BẰNG TIẾNG ANH DO LUA CHỈ CÓ NGÔN NGỮ KIỂU ASCII.
--CÔNG CỤ--
SoundEngine = "XAudio2" --"XAudio2" hay "DirectSound"?

--ÂM THANH--
Audio = {
	Intro = {Loopable = false, File = "./audio/Intro.wav"},
	MainMenu = {Loopable = true, File = "./audio/MainMenu.wav"}, --LOOP
	gui_click = {Loopable = false, File = "./audio/gui-click.wav"},
	gui_close = {Loopable = false, File = "./audio/gui-close.wav"},
	camera_shutter = {Loopable = false, File = "./audio/camera-shutter.wav"},
	MetalWalking = {Loopable = false, File = "./audio/walking.wav"},
	AsphaltWalking = {Loopable = false, File = "./audio/asphalt_walking.wav"}
}

--HÌNH ẢNH--
Textures = {
	Title = "./image/title.png",
	Player = "./image/player.png",

	--HÌNH GẠCH--
	GrassTile = "./image/GrassTile.png",
	ConcreteTile = "./image/HalfLifeConcrete.jpg",
	AsphaltTile = "./image/Asphalt_nhua_duong.png",

	--HÌNH DỤNG CỤ--
	Camcorder = "./image/CamcorderIcon.png",

	--THỰC THỂ--
	WoodenBlock = "./image/block.png"
}

TileTextures = { --CHỈ BIẾN THAM KHẢO NHỮNG HÌNH ẢNH
	"GrassTile", "ConcreteTile", "AsphaltTile"
}

--DANH SÁCH TIẾNG KHI NHÂN VẬT BƯỚC VÀO HÌNH GẠCH--
sound_onwalkingtiles =
{
	{Texture = "ConcreteTile", Audio = "MetalWalking"},
	{Texture = "AsphaltTile", Audio = "AsphaltWalking"}
}

other_locale = {"en"} --EN: Primary language is Vietnamese