extends HBoxContainer

# This node will give information on the state of the game : FPS, position, velocity ....

# Nodes
onready var Fonction_Onde = get_tree().get_root().get_node("Level").get_node("Fonction_Onde");
#onready var Player = get_tree().get_root().get_node("Level/Player");
onready var World_Info_Label = $LeftInfo/World_Info_Label;
onready var Player_Info_Label = $RightInfo/Player_Info_Label;
onready var Player : KinematicBody2D = get_tree().get_root().get_node("Level").get_node("Player")

func _ready():
	rect_size = Player.get_node("Camera2D").zoom * get_tree().get_root().size;
	pass


# Execute ASAP
func _process(delta):
#	Set position to the top the visible space (camera)	
	rect_position[0] = round(Player.position[0] - Player.get_node("Camera2D").zoom[0] * get_tree().get_root().size[0] /2)
	rect_position[1] = round(Player.position[1] - Player.get_node("Camera2D").zoom[1] * get_tree().get_root().size[1] /2)
	pass


# Execute Regularly
func _physics_process(delta):
#	Print out the text
#	World
	World_Info_Label.text = "FPS : " + str(1/delta) + "\n";
	World_Info_Label.text += "Wave function precision : N = " + str(Fonction_Onde.N) + "\n";
#	Player
	Player_Info_Label.text = "Position = " + str(Vector2(round(Player.position[0]), round(Player.position[1]))) + "\n";
	Player_Info_Label.text += "Velocity = " + str(Vector2(round(Player.velocity[0]), round(Player.velocity[1]))) + "\n";
	Player_Info_Label.text += "is on ground : " + str(Player.pstate == Player.PSTATE.ground) + "\n";
	Player_Info_Label.text += "is on wall : " + str(Player.is_on_wall()) + "\n";
	Player_Info_Label.text += "is on ledge : " + str(Player.is_on_ledge()) + "\n";
	Player_Info_Label.text += "Top collision: " + str(Player.get_collision_mask_bit(10)) + "\n";
	Player_Info_Label.text += "Energy: " + str(Player.energy) + "\n";

