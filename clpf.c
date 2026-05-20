#include <clpf.h>

static void lpf_emit_debug_print(lpf_t * lpf, const char * message)
{
    lpf->_log_fn(message);
}

void setup_lpf(lpf_t * lpf, uint8_t shift_amount)
{
    assert(shift_amount <= 16);
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

void setup_comparator_prints(lpf_t * lpf, const char* name, lpf_log_fn_t log_fn)
{
    if (lpf == NULL)
        return;

    memset(lpf->_name, 0, sizeof(lpf->_name));

    if (name != NULL)
        strncpy(lpf->_name, name, sizeof(lpf->_name) - 1);

    lpf->_name[sizeof(lpf->_name) - 1] = '\0';

    lpf->_log_fn = log_fn;
}

int32_t apply_filter(lpf_t * lpf, int32_t new_sample)
{
    lpf->raw_val = new_sample;

    if(lpf->_shift_amount == 0)
    {
        lpf->_reg = new_sample;
        lpf->filtered_val = new_sample;
    }
    else
    {
        lpf->_reg = lpf->_reg - (lpf->_reg >> lpf->_shift_amount) + new_sample;
        lpf->filtered_val = lpf->_reg >> lpf->_shift_amount;
    }

    return lpf->filtered_val;
}

uint8_t apply_comparator(lpf_t * lpf)
{
    if(!lpf->_is_comparator_set)
        return 0;

    uint32_t now = LPF_GET_TIME();

    if(!lpf->_initialized)
    {
        lpf->last_inrange_time = now;
        lpf->_initialized = 1;
    }

    uint8_t out_of_range;
    if(!lpf->_should_inverse)
        out_of_range = (lpf->filtered_val > lpf->_threshold);
    else
        out_of_range = (lpf->filtered_val < lpf->_threshold);

    if(!out_of_range)
        lpf->last_inrange_time = now;

    lpf->prev_detection = lpf->is_detected;

    if(lpf->_above_th_time == 0)
        lpf->is_detected = out_of_range;
    else
        lpf->is_detected = out_of_range && ((now - lpf->last_inrange_time) >= lpf->_above_th_time);

    lpf->rising_edge = 0;
    lpf->falling_edge = 0;

    if(lpf->prev_detection != lpf->is_detected)
    {
        lpf->state_changed = 1;
        lpf->last_change_time = now;

        if(lpf->is_detected)
            lpf->rising_edge = 1;
        else
            lpf->falling_edge = 1;

        if(lpf->_log_fn != NULL)
        {
            char p[64] = {0};
            snprintf(p, sizeof(p), "%s state=%d\r\n", lpf->_name, (int)lpf->is_detected);
            lpf_emit_debug_print(lpf, p);
        }
    }
    else
        lpf->state_changed = 0;

    return lpf->is_detected;
}


void debug_lpf(lpf_t * lpf)
{
    char p[128] = {0};
    snprintf(p, sizeof(p), ">%s: %ld\r\n>%s filtered: %ld\r\n", lpf->_name, (long)lpf->raw_val, lpf->_name, (long)lpf->filtered_val);
    lpf_emit_debug_print(lpf, p);
}

//-------------------------------------------------------------