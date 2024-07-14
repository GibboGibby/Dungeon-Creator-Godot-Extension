extends DungeonCreator
@onready var http_request: HTTPRequest = $HTTPRequest


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var hasConnection = has_internet_connection()
	if hasConnection:
		var imageUrl = create_gpt_image("A skeleton from a 90s video game")
		print(imageUrl)
		#download_from_url(imageUrl)
		#download_from_url_request(imageUrl, http_request)
		http_request.request(imageUrl)
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass


func _on_request_completed(result: int, response_code: int, headers: PackedStringArray, body: PackedByteArray):
	print("on_request_completed")
	var file = FileAccess.open("res://ai_images/downloaded_image_skeleton.png", FileAccess.WRITE)
	file.store_buffer(body)
	file.close()
	print("finished on_request_completed")
	
