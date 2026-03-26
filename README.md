# clpf

A lightweight C low-pass filter library using bit-shift averaging. Optionally includes a comparator to detect when the filtered value stays above a threshold for a set amount of time.


## Filter behavior

The filter uses an exponential moving average implemented with integer bit shifts, which makes it cheap on small MCUs with no FPU.

| `shift_amount` | Rise time (samples) |
|:-:|:-:|
| 1 | 3 |
| 2 | 8 |
| 3 | 16 |
| 4 | 34 |
| 5 | 69 |
| 6 | 140 |
| 7 | 280 |
| 8 | 561 |


## Usage

```c
#include <clpf.h>

lpf_t my_filter;

// Initialize — call once before use
setup_lpf(&my_filter, 4);                          // shift_amount = 4 (~34 sample rise time)
setup_comparator(&my_filter, 5000, 0, 200);        // threshold=5000, normal direction, 200ms hold time
setup_comparator_prints(&my_filter, "my_filter");  

// In your main loop
int32_t filtered = apply_filter(&my_filter, raw_sample);
uint8_t detected = apply_comparator(&my_filter);

if (my_filter.state_changed) {
    // rising or falling edge detected
}

debug_lpf(&my_filter);
```

## API

| Function | Description |
|---|---|
| `setup_lpf(lpf, shift_amount)` | Initialize the filter. Must be called first. |
| `setup_comparator(lpf, threshold, should_inverse, above_th_time_ms)` | Configure threshold detection. `should_inverse=1` detects below threshold. |
| `setup_comparator_prints(lpf, name)` | Enable debug output with a label name. |
| `apply_filter(lpf, sample)` | Feed a new sample, returns the filtered value. |
| `apply_comparator(lpf)` | Evaluate threshold, returns `1` when detected. |
| `debug_lpf(lpf)` | Print raw and filtered values |

