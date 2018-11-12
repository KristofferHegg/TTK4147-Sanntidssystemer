#ifndef PID_H
#define PID_H

typedef struct {
	float Kp;
	float Ki;
	float Kd;
	float dt;

	float setpoint;
	float process_value;

	float i_error;
	float i_error_max;
	float i_error_min;
	float prev_error;

	float control_signal;

} PID_controller;


PID_controller* pid_new(float Kp, float Ki, float Kd, float dt);
void pid_set_setpoint(PID_controller* c, float sp);
void pid_set_process_value(PID_controller* c, float pv);
void pid_calculate_control_signal(PID_controller* c);
void pid_set_delta_time();
float pid_get_control_signal(PID_controller* c);

#endif /* PID_H */
