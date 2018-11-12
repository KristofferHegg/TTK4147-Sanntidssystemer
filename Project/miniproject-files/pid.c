#include <stdlib.h>
#include <stdio.h>
#include "pid.h"

PID_controller* pid_new(float Kp, float Ki, float Kd, float dt) {
	PID_controller* c = (PID_controller*)malloc(sizeof(PID_controller));

	c -> Kp = Kp;
	c -> Ki = Ki;
	c -> Kd = Kd;
	c -> dt = dt;

	c -> setpoint = 0.0;
	c -> process_value = 0.0;

	c -> i_error = 0.0;
	c -> i_error_max = 2.0;
	c -> i_error_min = -2.0;
	c -> prev_error = 0.0;

	c -> control_signal = 0.0;

	return c;
}

void pid_set_setpoint(PID_controller* c, float sp){
	c -> setpoint = sp;
}

void pid_set_process_value(PID_controller* c, float pv){
	c -> process_value = pv;
}

void pid_calculate_control_signal(PID_controller* c){

	// Calculate error
	float error = (c -> setpoint) - (c -> process_value);
	(c -> i_error) += error * (c -> dt);
	float error_delta = error - (c -> prev_error);

	// Save error to previous error
	(c -> prev_error) = error;
	
	// Eliminate integral windup
	if((c -> i_error) > (c -> i_error_max)){
		(c -> i_error) = (c -> i_error_max);	
	} else if ((c -> i_error) < (c -> i_error_min)){
		(c -> i_error) = (c -> i_error_min);
	}
	
	// Proportional term
	float p_term = (c -> Kp) * error;

	// Integral term
	float i_term = (c -> Ki) * (c -> i_error) * (c -> dt);

	// Derivative term
	float d_term = (c -> Kd) * (error_delta / (c -> dt));

	// Write control output
	c -> control_signal = p_term + i_term + d_term;
}

float pid_get_control_signal(PID_controller* c){
	return c -> control_signal;
}
