#include <stdint.h>
#include "yeekbot_smoke.h"

/* revision C: reintroduce an unbounded length */
void collect_window(const uint8_t *adc, uint32_t adc_len)
{
    ingest_samples(adc, adc_len);
}
