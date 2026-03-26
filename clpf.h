#ifndef lpf_h
#define lpf_h


#ifdef ARDUINO
    #include <Arduino.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/*
	filter rise time (samples)
	k = 1 -> 3
	k = 2 -> 8
	k = 3 -> 16
	k = 4 -> 34
	k = 5 -> 69
	k = 6 -> 140
	k = 7 -> 280
	k = 8 -> 561
*/


#ifdef ARDUINO
	void lpf_platform_debug_print(const char * message);
	#define LPF_GET_TIME() millis()
	#define LPF_DEBUG_PRINT(message) lpf_platform_debug_print(message)
#else
	#define LPF_GET_TIME() ((uint32_t)0)
	#define LPF_DEBUG_PRINT(message) do { (void)(message); } while (0)
#endif


typedef struct
{
	//lpf parameters
	char _name[30];
	uint8_t _shift_amount;
	int32_t _threshold;
	uint8_t _should_inverse;
	uint32_t _above_th_time;

	int32_t _reg;
	int32_t raw_val;
    int32_t filtered_val;

	uint8_t is_detected, prev_detection, state_changed;
	uint32_t last_inrange_time, last_change_time;

	uint8_t _is_comparator_set, _is_debug_print_set;

} lpf_t;


void setup_lpf(lpf_t * lpf, uint8_t shift_amount);
void setup_comparator(lpf_t * lpf, int32_t threshold, uint8_t should_inverse, uint32_t above_th_time);
void setup_comparator_prints(lpf_t * lpf, const char* name);


int32_t apply_filter(lpf_t * lpf, int32_t new_sample);
uint8_t apply_comparator(lpf_t * lpf);

void debug_lpf(lpf_t * lpf);

#ifdef __cplusplus
}
#endif

#endif 