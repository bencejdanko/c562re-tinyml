#include <stdint.h>
#include "yeekbot_smoke.h"

/* revision B - bounds the length */
void collect_window(const uint8_t *adc, uint32_t adc_len)
{
    if (adc_len > SENSOR_BUF_LEN) {
        adc_len = SENSOR_BUF_LEN;
    }
    ingest_samples(adc, adc_len);
}
