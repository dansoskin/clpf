#include <lpf.h>

void setup_lpf(lpf_t * lpf, char* name, uint8_t shift_amount, int16_t threshold, uint8_t should_inverse, uint16_t above_th_time)
{
    memset(lpf->_name, 0, sizeof(lpf->_name));
    strncpy(lpf->_name, name, sizeof(lpf->_name));
    
    lpf->_shift_amount = shift_amount;
    lpf->_threshold = threshold;
    lpf->_should_inverse = should_inverse;
    lpf->_above_th_time = above_th_time;
}


uint16_t apply_filter(lpf_t * lpf, int16_t new_sample)
{
    lpf->raw_val = new_sample;
    lpf->_reg = lpf->_reg - (lpf->_reg >> lpf->_shift_amount) + new_sample;
    lpf->filtered_val = lpf->_reg >> lpf->_shift_amount;

    return lpf->filtered_val;
}

uint8_t compare_to_threshold(lpf_t * lpf)
{
    if(!lpf->_should_inverse)
    {
        if(lpf->filtered_val <= lpf->_threshold)
            lpf->last_inrange_time = millis();
    }
    else
    {
        if(lpf->filtered_val >= lpf->_threshold)
            lpf->last_inrange_time = millis();
    }
    
    lpf->prev_detection = lpf->is_detected;

    if(millis() - lpf->last_inrange_time >= (lpf->_above_th_time + 2))
        lpf->is_detected = 1;
    else
        lpf->is_detected = 0;


    if(lpf->prev_detection != lpf->is_detected)
    {
        lpf->state_changed = 1;
        lpf->last_change_time = millis();

        #ifdef DBG_LPF
            char p[30] = {0};
            snprintf(p, 30, "%s state=%u\n", lpf->_name, lpf->is_detected);
            Serial.println(p);
        #endif
    }
    else
        lpf->state_changed = 0;


    return lpf->is_detected;
}

//-------------------------------------------------------------