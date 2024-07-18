extends Camera2D

@export var SPEED : float = 200
@export var ZOOM_SPEED : float = 1

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if Input.is_action_pressed("up"):
		position.y -= SPEED * delta
	if Input.is_action_pressed("down"):
		position.y += SPEED * delta
	if Input.is_action_pressed("left"):
		position.x -= SPEED * delta
	if Input.is_action_pressed("right"):
		position.x += SPEED * delta
	if Input.is_action_pressed("zoom_in"):
		zoom.x = zoom.x + (ZOOM_SPEED * delta)
		zoom.y = zoom.y + (ZOOM_SPEED * delta)
	if Input.is_action_pressed("zoom_out"):
		zoom.x = zoom.x - (ZOOM_SPEED * delta)
		zoom.y = zoom.y - (ZOOM_SPEED * delta)
