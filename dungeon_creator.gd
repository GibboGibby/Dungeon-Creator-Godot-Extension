extends DungeonCreator
@onready var http_request: HTTPRequest = $HTTPRequest
@onready var tile_map: TileMap = $TileMap

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	generate_tilemap_edge(tile_map)
	DoAllTheGenerating()
	
	#GenAndDownloadImage()
	#var image_url = create_gpt_image_3("can you generate a 2d side scrolling platformer tilset. It is a 2x3 tileset where: (0,0) is dirt, (1,0) is stone, (2,0) is the outer wall, (0,1) is a ladder, (1,1) is the top of a ladder and (2,1) is empty. With the theme of Hell")
	#print(image_url)

func DoAllTheGenerating():
	generate_dungeon()
	generate_tiles(tile_map)

func GenAndDownloadImage():
	var hasConnection = has_internet_connection()
	if hasConnection:
		print("I am generating the image")
		#var imageUrl = create_gpt_image_3("can you generate a 2d side scrolling platformer tilset. It is a 2x3 tileset where: (0,0) is dirt, (1,0) is stone, (2,0) is the outer wall, (0,1) is a ladder, (1,1) is the top of a ladder and (2,1) is empty. With the theme of Hell")
		var imageUrl = create_gpt_image_3("can you generate a 2d side scrolling platformer sprite tilset similar to spelunky with the theme of Hell")
		print("Finished generating the image")
		print(imageUrl)
		http_request.request(imageUrl)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if Input.is_action_pressed("NewDungeon"):
		DoAllTheGenerating()
	if Input.is_action_pressed("SDGen"):
		RunSDGen("seamless interesting mossy stone bricks")
		print("finished generating")
		
		
	if Input.is_action_just_pressed("ReloadTileset"):
		print("testing")
		
		print("User data Dir")
		print(OS.get_user_data_dir())
		print("Whether file exists or not")
		print(FileAccess.file_exists("user://combined_tileset.png"))
		var image = Image.load_from_file("user://combined_tileset.png")
		var texture = ImageTexture.create_from_image(image)
		var atlasSource = TileSetAtlasSource.new()
		atlasSource.texture = texture
		var tileSize = Vector2i(64,64)
		atlasSource.texture_region_size = tileSize
		atlasSource.separation = Vector2i(0,0)
		atlasSource.margins = Vector2i(0,0)
		atlasSource.create_tile(Vector2i(0,0))
		atlasSource.create_tile(Vector2i(0,1))
		atlasSource.create_tile(Vector2i(1,0))
		atlasSource.create_tile(Vector2i(1,1))
		
		tile_map.tile_set.remove_source(1)
		tile_map.tile_set.add_source(atlasSource, 1)
		
		
		
		'''
		for y in range(texture.get_height() / int(tileSize.y)):
			for x in range(texture.get_width() / int(tileSize.x)):
				var tile_id = y * (texture.get_width() / int(tileSize.x)) + x
				atlasSource.create_tile(tile_id)
				atlasSource.
		'''
		
		#var tileset = tile_map.tile_set.add_source()
		
		#tileset.add_source()
		
		#tile_map.tile_set.get_source(1).resource_path = "user://combined_tileset.png"
		
	pass


func _on_request_completed(result: int, response_code: int, headers: PackedStringArray, body: PackedByteArray):
	print("on_request_completed")
	var i = 1
	var baseFileLocation = "res://ai_images/test_tileset.png"
	while (FileAccess.file_exists(baseFileLocation)):
		baseFileLocation = "res://ai_images/test_tileset_" + str(i) + ".png"
		
		
	var file = FileAccess.open(baseFileLocation, FileAccess.WRITE)
	file.store_buffer(body)
	file.close()
	print("finished on_request_completed")
	
