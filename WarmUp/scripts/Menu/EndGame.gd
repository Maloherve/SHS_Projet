extends Control


# Declare member variables here. Examples:
onready var video_player = get_node("VideoPlayer")
onready var video = preload("res://assets/Background/tv_effect.ogv")



# Called when the node enters the scene tree for the first time.
func _ready():
	video_player.set_stream( video )
	set_process( true )


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if not video_player.is_playing():
		video_player.play()


func _on_Restart_pressed():
	$Click.play()
	SceneChanger.close_menu(0.5, "intro");


func _on_Restart_mouse_entered():
	$Menu/CenterRow/Buttons/Restart/Label.add_color_override("font_color", Color(0.9,0.1,0.9,0.5))


func _on_Restart_mouse_exited():
	$Menu/CenterRow/Buttons/Restart/Label.add_color_override("font_color", Color(1,1,1,1))

func _on_Quit_pressed():
	$Click.play()
	get_tree().quit();


func _on_Quit_mouse_entered():
	$Menu/CenterRow/Buttons/Quit/Label.add_color_override("font_color", Color(0.9,0.1,0.9,0.5))


func _on_Quit_mouse_exited():
	$Menu/CenterRow/Buttons/Quit/Label.add_color_override("font_color", Color(1,1,1,1))