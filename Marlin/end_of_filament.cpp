#include "end_of_filament.h"
#include "Configuration.h"
#include "stepper.h"

bool old_f_max_endstop = false;
bool end_of_filament_detection_report_end_of_filament_event = DEFAULT_FILAMENT_DETECTION_REPORT_END_OF_FILAMENT_EVENT;
bool end_of_filament_detection_request_pause = DEFAULT_FILAMENT_DETECTION_REQUEST_PAUSE;
bool end_of_filament_detection_call_m600 = DEFAULT_FILAMENT_DETECTION_CALL_M600;
typedef struct {
    bool stored;
    float current_position[NUM_AXIS];
    float continuation_position[NUM_AXIS];
} last_stored_state_t;
last_stored_state_t last_stored_state = { false };

void check_end_of_filament_endstop() {
  #if defined(F_MAX_PIN) && F_MAX_PIN > -1
    if (!last_stored_state.stored){
      bool f_max_endstop=(READ(F_MAX_PIN));
      if(f_max_endstop && !old_f_max_endstop){
        if (end_of_filament_detection_report_end_of_filament_event) {
          // notify end of filament with "Event:", which is better but requires a plugin
          SERIAL_PROTOCOLPGM("Event:EndOfFilament");
          SERIAL_PROTOCOLLN("");
        }
        if (end_of_filament_detection_request_pause) {
          // notify end of filament using Repetier Host pause
          SERIAL_PROTOCOLPGM("RequestPause:");
          SERIAL_PROTOCOLLN("");
        }
        if (end_of_filament_detection_call_m600) {
          // execute M600, which will cause the printer to park and ask the user to change the filament
          enquecommand_P(PSTR("M600"));
        }
      }
      old_f_max_endstop = f_max_endstop;
    }
  #endif
}
void set_end_of_filament_detection_report_end_of_filament_event(bool check){
  end_of_filament_detection_report_end_of_filament_event = check;
}
void set_end_of_filament_detection_request_pause(bool check){
  end_of_filament_detection_request_pause = check;
}
void set_end_of_filament_detection_call_m600(bool check){
  end_of_filament_detection_call_m600 = check;
}
void store_current_state_ext(float * current_position, float * continuation_position){
  st_synchronize();
  last_stored_state.stored = true;
  last_stored_state.current_position[X_AXIS] = current_position[X_AXIS];
  last_stored_state.current_position[Y_AXIS] = current_position[Y_AXIS];
  last_stored_state.current_position[Z_AXIS] = current_position[Z_AXIS];
  last_stored_state.current_position[E_AXIS] = current_position[E_AXIS];
  last_stored_state.continuation_position[X_AXIS] = continuation_position[X_AXIS];
  last_stored_state.continuation_position[Y_AXIS] = continuation_position[Y_AXIS];
  last_stored_state.continuation_position[Z_AXIS] = continuation_position[Z_AXIS];
  last_stored_state.continuation_position[E_AXIS] = continuation_position[E_AXIS];
}
void store_current_state(){
  store_current_state_ext(current_position, current_position);
}
void restore_last_state_stored(){
 if (last_stored_state.stored) {
    int feedrate = homing_feedrate[X_AXIS];
    if(homing_feedrate[Y_AXIS]<feedrate){
        feedrate = homing_feedrate[Y_AXIS];
    }
    last_stored_state.stored = false;
    // It's assumed that once you have extruded some filament, the filament will be in
    // the edge of the nozzle, so, no extrusion or retraction will be done.
    current_position[E_AXIS] = last_stored_state.continuation_position[E_AXIS];
    plan_set_e_position(current_position[E_AXIS]);
    plan_buffer_line(last_stored_state.current_position[X_AXIS], last_stored_state.current_position[Y_AXIS], last_stored_state.current_position[Z_AXIS], last_stored_state.continuation_position[E_AXIS], feedrate/60, active_extruder); //should do nothing
    plan_buffer_line(last_stored_state.continuation_position[X_AXIS], last_stored_state.continuation_position[Y_AXIS], last_stored_state.current_position[Z_AXIS], last_stored_state.continuation_position[E_AXIS], feedrate/60, active_extruder); //move xy back
    plan_buffer_line(last_stored_state.continuation_position[X_AXIS], last_stored_state.continuation_position[Y_AXIS], last_stored_state.continuation_position[Z_AXIS], last_stored_state.continuation_position[E_AXIS], max_feedrate[Z_AXIS], active_extruder); //move z back
    plan_buffer_line(last_stored_state.continuation_position[X_AXIS], last_stored_state.continuation_position[Y_AXIS], last_stored_state.continuation_position[Z_AXIS], last_stored_state.continuation_position[E_AXIS], feedrate/60, active_extruder); //final untretract
    current_position[X_AXIS] = last_stored_state.continuation_position[X_AXIS];
    current_position[Y_AXIS] = last_stored_state.continuation_position[Y_AXIS];
    current_position[Z_AXIS] = last_stored_state.continuation_position[Z_AXIS];
    current_position[E_AXIS] = last_stored_state.continuation_position[E_AXIS];
  }   
}
bool is_state_stored(){
  return last_stored_state.stored;
}
