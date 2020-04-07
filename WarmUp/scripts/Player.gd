extends KinematicBody2D

# Node describing the movement and actions of the player

# CONSTANTS
const TYPE = "PLAYER";
export var cat : bool = false;
const GRAVITY : int = 800;
const MOVE_SPEED : int = 1 * 64;
const CLIMB_SPEED : float = 1.5 * 64;
# Nodes
onready var animNode : AnimatedSprite;
onready var Bottom_Raycasts : Node2D = get_node("Bottom_Raycasts");
onready var Side_Raycasts : Node2D = get_node("Side_Raycasts");
# Movement
var velocity : Vector2 = Vector2();
var jump_velocity : int = -388;
var Up : Vector2 = Vector2(0, -1);
var move_direction : int = 1;
var vertical_move_direction : int = 1;
var is_on_wall : bool = false;
var is_on_ledge : bool = false;
# Attack
var attack_move_direction : int = 1;
var attack : bool = false;
var attackstun : int = 0;
var meleeTime : int = 30;
var hitstun : int = 0;
# Cast
var cast : bool = false;
var is_casting : bool = false;
var finish_cast : bool = false;
# Audio
var myAudio : AudioStreamPlayer2D;
var myAudioIdle : AudioStreamPlayer2D;
# Signals
signal start_casting;
signal is_casting
signal stop_casting;
signal finish_casting;


func _ready():
	if (animNode != null):
		animNode.play("_idle");

	if (!cat):
		animNode = get_node("AnimatedSprite_Human");
		$AnimatedSprite_Cat.hide();
		$CollisionShape2D_Cat.disabled = true;
	else:
		animNode = get_node("AnimatedSprite_Cat");
		$AnimatedSprite_Human.hide();
		$CollisionShape2D_Human.disabled = true;

	# Connect Signals
	animNode.connect("animation_finished", self, "_on_AnimatedSprite_animation_finished");


# Execute ASAP
func _process(delta):
	_assign_animation();
	if !attack:
		_get_input();
		MoveCharacter(delta);
		if (move_direction == -1):
			animNode.set_flip_h(false);
			Side_Raycasts.set_flip(true);
		elif (move_direction == 1):
			animNode.set_flip_h(true);
			Side_Raycasts.set_flip(false);


# Execute Regularly
func _physics_process(delta):
	if !cast:
		velocity = move_and_slide(velocity, Vector2(0,0));


# Move Character
func MoveCharacter(delta):
#	Determine the new Velocities
#	x-Movement
	if (!attack && !cast):
		velocity.x = lerp(velocity.x, MOVE_SPEED * move_direction, 0.2);
	
#	y-Movement
	if is_on_wall:
		velocity.y = lerp(velocity.y, CLIMB_SPEED * vertical_move_direction, 0.2);
		if !_check_is_collided():
			is_on_wall = false;
	
#	Gravity
	if _check_if_apply_gravity():
		velocity.y += GRAVITY * delta;


# Check for and execute Input
func _get_input():
	if (Input.is_action_just_pressed("ui_up") && !cast):
		if _check_is_grounded():
			velocity.y = jump_velocity;
		elif is_on_ledge:
			velocity.y = jump_velocity; 
	if (Input.is_action_just_pressed("ui_down") && _check_is_grounded() && !cast):
		attack = true;
		attackstun = meleeTime;
	if (Input.is_action_just_pressed("ui_space") && (!is_casting)):
		emit_signal('start_casting');
		cast = true;
	if (Input.is_action_just_pressed("ui_space") && (is_casting)):
		emit_signal("stop_casting");
		finish_cast = true;
	
	# Update move direction
	if (!cast):
		move_direction = -int(Input.is_action_pressed("ui_left")) + int(Input.is_action_pressed("ui_right"));
		if is_on_wall: vertical_move_direction = -int(Input.is_action_pressed("ui_up")) + int(Input.is_action_pressed("ui_down"));
		if((move_direction != attack_move_direction) && move_direction!=0):
			attack_move_direction = move_direction;


# Use Raycast to see if the character is on the ground
func _check_is_grounded():
	for raycast in Bottom_Raycasts.get_children():
		if raycast.is_colliding():
			return true;
	return false;


# See if the character next to a wall
func _check_is_collided():
	if Side_Raycasts.get_node("Bottom_Side").is_colliding() && ! Side_Raycasts.get_node("Top_Side").is_colliding():
		is_on_ledge = true;
	else:
		is_on_ledge = false;
	for raycast in Side_Raycasts.get_children():
		if raycast.is_colliding():
			return true;
	return false;


# Check if the conditions to apply gravity are verified
func _check_if_apply_gravity():
	if cast:
		return false;
	if (!_check_is_grounded() && _check_is_collided()):
		is_on_wall = true;
		return false;
	return true;


# Assign and play animation
func _assign_animation():
	var anim = "_idle";
	if (!attack && !cast):
		if (!_check_is_grounded() && hitstun == 0):
			if (is_on_wall):
				anim = "_hold";
			else:
				anim = "_jump";
		elif (move_direction != 0):
			anim = "_walk";
		elif (hitstun > 0):
			anim = "_damage";
	elif (attack):
		anim = "_attack";
	elif (cast):
		if (finish_cast):
			anim = "_endcast"
		else:
			anim = "_cast";
	
	if (animNode.get_animation() != anim):
		animNode.play(anim);


# ----- Node Function ------
func _on_AnimatedSprite_animation_finished():
	if (animNode.get_animation() == "_attack"):
		attackstun = meleeTime;
		attack = false;
	if (animNode.get_animation() == "_cast"):
		emit_signal("is_casting");
		is_casting = true;
	if (animNode.get_animation() == "_endcast"):
		emit_signal("finish_casting");
		finish_cast = false;
		is_casting = false;
		cast = false;