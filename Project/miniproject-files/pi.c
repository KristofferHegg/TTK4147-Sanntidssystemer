#include "pi.h"

float kp = 4.5;
float ki = 200.0;
float kd = 0.00;

float i_error = 0;
float i_error_max = 5;
float i_error_min = -5;

float prev_error = 0.0;

float PI(float sp, float pv, float dt){
	
	float error = sp - pv;	
	i_error += error * dt;
	float error_delta = error - prev_error;

	// Eliminate integral windup
	if(i_error > i_error_max){
		i_error = i_error_max;	
	} else if (i_error < i_error_min){
		i_error = i_error_min;
	}	

	float p_term = kp * error;
	float d_term = kd * (error_delta / dt);		
	float i_term = ki * i_error;

	float output = p_term + i_term + d_term;

	prev_error = error;

	return output;
}
