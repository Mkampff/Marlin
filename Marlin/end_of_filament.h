#ifndef end_of_filament_h
#define end_of_filament_h

// Configuration
extern bool end_of_filament_detection_report_end_of_filament_event;
extern bool end_of_filament_detection_request_pause;
extern bool end_of_filament_detection_call_m600;

void check_end_of_filament_endstop();
void set_end_of_filament_detection_report_end_of_filament_event(bool check);
void set_end_of_filament_detection_request_pause(bool check);
void set_end_of_filament_detection_call_m600(bool check);

void store_current_state_ext(float * current_position, float * continuation_position);
void store_current_state();
void restore_last_state_stored();
void clear_state_stored();
bool is_state_stored();
  

#endif
