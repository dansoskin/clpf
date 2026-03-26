#include <clpf.h>



void setup_lpf(lpf_t * lpf, uint8_t shift_amount)
{
    memset(lpf, 0, sizeof(lpf_t));   
    lpf->_shift_amount = shift_amount;
}

void setup_comparator(lpf_t * lpf, int32_t threshold, uint8_t should_inverse, uint32_t above_th_time)
{
    lpf->_threshold = threshold;
    lpf->_should_inverse = should_inverse;
    lpf->_above_th_time = above_th_time;
    
    lpf->_is_comparator_set = 1;
}

void setup_comparator_prints(lpf_t * lpf, const char* name)
{
    memset(lpf->_name, 0, sizeof(lpf->_name));
    // Ensure name is copied and null-terminated
    strncpy(lpf->_name, name, sizeof(lpf->_name) - 1);
    lpf->_name[sizeof(lpf->_name) - 1] = '\0';

    lpf->_is_debug_print_set = 1;
}


int32_t apply_filter(lpf_t * lpf, int32_t new_sample)
{
    lpf->raw_val = new_sample;
    lpf->_reg = lpf->_reg - (lpf->_reg >> lpf->_shift_amount) + new_sample;
    lpf->filtered_val = lpf->_reg >> lpf->_shift_amount;

    return lpf->filtered_val;
}

uint8_t apply_comparator(lpf_t * lpf)
{
    if(!lpf->_is_comparator_set)
        return 0;
    
    uint32_t now = LPF_GET_TIME();
    if(!lpf->_should_inverse)
    {
        if(lpf->filtered_val <= lpf->_threshold)
            lpf->last_inrange_time = now;
    }
    else
    {
        if(lpf->filtered_val >= lpf->_threshold)
            lpf->last_inrange_time = now;
    }
    
    lpf->prev_detection = lpf->is_detected;

    if(now - lpf->last_inrange_time >= (lpf->_above_th_time))
        lpf->is_detected = 1;
    else
        lpf->is_detected = 0;


    if(lpf->prev_detection != lpf->is_detected)
    {
        lpf->state_changed = 1;
        lpf->last_change_time = now;

        if(lpf->_is_debug_print_set)
        {
            char p[64] = {0};
            snprintf(p, sizeof(p), "%s state=%d", lpf->_name, (int)lpf->is_detected);
            LPF_DEBUG_PRINT(p);
        }
    }
    else
        lpf->state_changed = 0;


    return lpf->is_detected;
}


void debug_lpf(lpf_t * lpf)
{
    char p[128] = {0};
    snprintf(p, sizeof(p), ">%s: %ld\r\n>%s filtered: %ld", lpf->_name, (long)lpf->raw_val, lpf->_name, (long)lpf->filtered_val);
    LPF_DEBUG_PRINT(p);
}

//-------------------------------------------------------------