# clpf

A lightweight C low-pass filter library using bit-shift averaging. Optionally includes a comparator to detect when the filtered value stays above a threshold for a set amount of time.

The library is intended mainly for Arduino and STM32 targets. Timekeeping is platform-specific through `millis()` on Arduino and `HAL_GetTick()` on STM32.


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

### Basic setup

```c
#include <clpf.h>

lpf_t my_filter;

setup_lpf(&my_filter, 4);                          // shift_amount = 4 (~34 sample rise time)
setup_comparator(&my_filter, 5000, 0, 200);       // threshold=5000, normal direction, 200 ms hold time

int32_t filtered = apply_filter(&my_filter, raw_sample);
uint8_t detected = apply_comparator(&my_filter);

if (my_filter.state_changed) {
    // rising or falling edge detected
}
```

### Debug logging callback

Debug output is now configured with a user-provided callback:

```c
void setup_comparator_prints(lpf_t *lpf, const char *name, lpf_log_fn_t log_fn);
```

The callback signature is:

```c
typedef void (*lpf_log_fn_t)(const char *message);
```

The library builds the message string internally and calls your function whenever comparator state changes or when `debug_lpf()` is called.

### Arduino example

```c
#include <clpf.h>

static void clpf_log(const char *message)
{
    Serial.println(message);
}

lpf_t my_filter;

void setup()
{
    Serial.begin(115200);

    setup_lpf(&my_filter, 4);
    setup_comparator(&my_filter, 5000, 0, 200);
    setup_comparator_prints(&my_filter, "impact", clpf_log);
}
```

### STM32 example

```c
#include <clpf.h>

extern myUART_t myUart;

static void clpf_log(const char *message)
{
    send_uart(&myUart, "%s", message);
}

lpf_t my_filter;

void setup_filter(void)
{
    setup_lpf(&my_filter, 4);
    setup_comparator(&my_filter, 5000, 0, 200);
    setup_comparator_prints(&my_filter, "impact", clpf_log);
}
```

### Notes

- Pass `NULL` as `log_fn` only if you do not intend to call `debug_lpf()` or use comparator debug prints.
- `should_inverse = 0` means detection occurs when the filtered value stays above threshold long enough.
- `should_inverse = 1` flips the comparison logic.
- `state_changed` is set for one evaluation cycle when detection changes state.

## API

| Function | Description |
|---|---|
| `setup_lpf(lpf, shift_amount)` | Initialize the filter. Must be called first. |
| `setup_comparator(lpf, threshold, should_inverse, above_th_time_ms)` | Configure threshold detection. `should_inverse=1` detects below threshold. |
| `setup_comparator_prints(lpf, name, log_fn)` | Enable debug output with a label name and a caller-provided log callback. |
| `apply_filter(lpf, sample)` | Feed a new sample, returns the filtered value. |
| `apply_comparator(lpf)` | Evaluate threshold, returns `1` when detected. |
| `debug_lpf(lpf)` | Send a debug message with raw and filtered values through the configured callback. |

