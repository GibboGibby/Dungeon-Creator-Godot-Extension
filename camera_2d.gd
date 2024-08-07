extends Camera2D

@export var SPEED : float = 200
@export var ZOOM_SPEED : float = 1
@export var player : CharacterBody2D
@export var shouldFollowPlayer := true

@export var minZoom := 0.5
@export var maxZoom := 2.5


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void: 
	
	if Input.is_action_just_pressed("mouse_wheel_down"):
		#print("mouse wheel down")
		zoom -= Vector2(0.1,0.1)
	if Input.is_action_just_pressed("mouse_wheel_up"):
		#print("mouse wheel up")
		zoom += Vector2(0.1,0.1)
	var zoomX = zoom.x
	zoomX = clamp(zoomX, minZoom, maxZoom)
	zoom = Vector2(zoomX, zoomX)
	if (shouldFollowPlayer):
		position = player.position
		return
