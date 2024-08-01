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
	
