class_name SpineAnimationEvent extends RefCounted

enum Type
{
	START      = 0,
	INTERRUPT  = 1,
	END        = 2,
	COMPLETE   = 3,
	DISPOSE    = 4,
	USER_EVENT = 5
}


var type:Type
var track_entry_id:int
var animation_name:String
var track_index:int
var user_event_name:Variant

func _init( type:Type, track_entry_id:int, animation_name:String, track_index:int, user_event_name:Variant ):
	self.type = type
	self.track_entry_id = track_entry_id
	self.animation_name = animation_name
	self.track_index = track_index
	self.user_event_name = user_event_name

func _to_string()->String:
	if not user_event_name: return "%s[%d] %s (%d)" % [animation_name,track_index,type_name(),track_entry_id]
	return "%s[%d] %s:%s (%d)" % [animation_name,track_index,type_name(),user_event_name,track_entry_id]

func type_name()->String:
	match type:
		Type.START:      return "START"
		Type.INTERRUPT:  return "INTERRUPT"
		Type.END:        return "END"
		Type.COMPLETE:   return "COMPLETE"
		Type.DISPOSE:    return "DISPOSE"
		Type.USER_EVENT: return "USER_EVENT"
		_:               return "UNKNOWN"

