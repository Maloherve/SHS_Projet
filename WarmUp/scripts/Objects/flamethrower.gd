extends Particles2D

export (float) var switch_time = 4.0;
export (float) var shift = 0.0;
onready var player : KinematicBody2D = get_tree().get_current_scene().get_node("Player");

var delay_state : GDScriptFunctionState = null;

func enable():
	self.emitting = true;
	$sound.playing = true;
	#delay_state = yield(get_tree().create_timer(0.4, false), "timeout");
	$damage_area.enabled = true;

func disable():
	self.emitting = false;
	$sound.playing = false;
	#delay_state = yield(get_tree().create_timer(0.8, false), "timeout");
	$damage_area.enabled = false;
	
func is_enabled():
	return self.emitting;
	
func _on_timer_timeout():
	if is_enabled():
		disable()
	else:
		enable()	

# Called when the node enters the scene tree for the first time.
func _ready():
	disable();
	if shift != 0:
		yield(get_tree().create_timer(shift), "timeout");
	$clock.connect("timeout", self, "_on_timer_timeout");
	$clock.start(switch_time);