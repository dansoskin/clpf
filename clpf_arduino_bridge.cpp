#include <Arduino.h>
#include <clpf.h>


extern "C" void lpf_platform_debug_print(const char * message)
{
    Serial.println(message);
}
