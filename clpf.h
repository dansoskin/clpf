#ifndef lpf_h
#define lpf_h

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>

#define DBG_LPF

typedef struct
{
	//lpf parameters
	char _name[30];
	uint8_t _shift_amount;
	int16_t _threshold;
	uint8_t _should_inverse;
	uint32_t _above_th_time;

	int32_t _reg;
	int16_t raw_val;
    int16_t filtered_val;

	uint8_t is_detected, prev_detection, state_changed;
	uint32_t last_inrange_time, last_change_time;

} lpf_t;


void setup_lpf(lpf_t * lpf, char* name, uint8_t shift_amount, int16_t threshold, uint8_t should_inverse, uint16_t above_th_time);
uint16_t apply_filter(lpf_t * lpf, int16_t new_sample);
uint8_t compare_to_threshold(lpf_t * lpf);

#ifdef __cplusplus
}
#endif

#endif 