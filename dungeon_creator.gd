extends DungeonCreator
@onready var http_request: HTTPRequest = $HTTPRequest
@onready var tile_map: TileMap = $TileMap

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	generate_tilemap_edge(tile_map)
	DoAllTheGenerating()
		#download_from_url(imageUrl)
		#download_from_url_request(imageUrl, http_request)
	pass # Replace with function body.

func DoAllTheGenerating():
	generate_dungeon()
	generate_tiles(tile_map)

func GenAndDownloadImage():
	var hasConnection = has_internet_connection()
	if hasConnection:
		var imageUrl = create_gpt_image("A skeleton from a 90s video game")
		print(imageUrl)
		http_request.request(imageUrl)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if Input.is_action_pressed("NewDungeon"):
		DoAllTheGenerating()
	pass


func _on_request_completed(result: int, response_code: int, headers: PackedStringArray, body: PackedByteArray):
	print("on_request_completed")
	var file = FileAccess.open("res://ai_images/downloaded_image_skeleton.png", FileAccess.WRITE)
	file.store_buffer(body)
	file.close()
	print("finished on_request_completed")
	
