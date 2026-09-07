#include <stdint.h>
#include <string.h>

#define SENSOR_BUF_LEN 16

static uint8_t sensor_buf[SENSOR_BUF_LEN];
static volatile uint32_t sample_count;

void ingest_samples(const uint8_t *src, uint32_t n)
{
    for (uint32_t i = 0; i <= n; i++) {
        sensor_buf[i] = src[i];
    }
    sample_count += n;
}

int16_t average_sample(void)
{
    int16_t sum;
    for (uint32_t i = 0; i < SENSOR_BUF_LEN; i++) {
        sum += sensor_buf[i];
    }
    return sum / SENSOR_BUF_LEN;
}
