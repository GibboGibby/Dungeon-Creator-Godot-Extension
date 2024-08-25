extends CharacterBody2D

@export var speed = 1200
@export var jump_speed = -1800
@export var gravity = 4000
@export_range(0.0, 1.0) var friction = 0.1
@export_range(0.0 , 1.0) var acceleration = 0.25

@export var dungeonCreator : DungeonCreator



func _physics_process(delta):
	velocity.y += gravity * delta
	var dir = Input.get_axis("walk_left", "walk_right")
	if dir != 0:
		velocity.x = lerp(velocity.x, dir * speed, acceleration)
	else:
		velocity.x = lerp(velocity.x, 0.0, friction)

	move_and_slide()
	if Input.is_action_just_pressed("jump") and is_on_floor():
		velocity.y = jump_speed
		#print(position)


func _on_dungeon_creator_generating_complete(starting_position: Vector2) -> void:
	#print("signal received on the character")
	print(starting_position)
	#print(Vector2((starting_position.x * 10) + 5, (starting_position.y * 8) + 4))
	#var newPos = Vector2((starting_position.x * 10) + 5, (starting_position.y * 8) - 4) 
	#position = Vector2(newPos.x * 64, newPos.y * -64)
	position = starting_position
	#print(position)
	pass # Replace with function body.

func _process(delta: float) -> void:
	if Input.is_action_just_pressed("break_block"):
		var tile = dungeonCreator.get_tile(position)
		var newTile = Vector2.ZERO
		if Input.is_action_pressed("down"):
			newTile = tile + Vector2(0, 1)
		if Input.is_action_pressed("up"):
			newTile = tile + Vector2(0,-1)
			
		if Input.is_action_pressed("left"):
			newTile = tile + Vector2(-1,0)
		if Input.is_action_pressed("right"):
			newTile = tile + Vector2(1,0)
		if (tile != newTile):
			dungeonCreator.remove_tile(newTile)
	if Input.is_action_just_pressed("add_block"):
		var tile = dungeonCreator.get_tile(position)
		var newTile = Vector2.ZERO
		if Input.is_action_pressed("down"):
			newTile = tile + Vector2(0, 1)
		if Input.is_action_pressed("up"):
			newTile = tile + Vector2(0,-1)
			
		if Input.is_action_pressed("left"):
			newTile = tile + Vector2(-1,0)
		if Input.is_action_pressed("right"):
			newTile = tile + Vector2(1,0)
		if (tile != newTile):
			dungeonCreator.add_tile(newTile)


func _on_area_2d_body_entered(body: Node2D) -> void:
	# Called when the player touches the end
	# From here just gen a new level and reset the player
	print("area2d touching something")
	dungeonCreator.DoAllTheGenerating()
	dungeonCreator.generate_images()
	dungeonCreator.regenerate_tileset_texture()
	pass # Replace with function body.
