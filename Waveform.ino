#include <Arduino_AdvancedAnalog.h>

#define N_SAMPLES (256)
#define DEFAULT_FREQUENCY (1024 * 256)

AdvancedDAC dac1(A12);
uint8_t SAMPLES_BUFFER[N_SAMPLES];

void setup()
{
    Serial.begin(115200);

    for (int i = 0; i < N_SAMPLES; i++)
    {
        SAMPLES_BUFFER[i] = sin(2 * 3.14 * (i / (float)N_SAMPLES)) * 127 + 127;
    }

    if (!dac1.begin(AN_RESOLUTION_8, DEFAULT_FREQUENCY, N_SAMPLES, 32))
    {
        Serial.println("Failed to start DAC1 !");
        while (1)
            ;
    }

    // set A0 as analog input
    pinMode(A0, INPUT);
}

void loop()
{
    static uint32_t counter = 0;
    static int last_reading = 0;
    if (dac1.available())
    {
        if (counter > 100)
        {
            counter = 0;

            int value = analogRead(A0);
            if (value != last_reading)
            {
                Serial.println(value);
                last_reading = value;
                // adjust the frequency of the DAC based on the analog value, scaling DEFAULT_FREQUENCY from 0.0 to 1.0
                dac1.frequency(DEFAULT_FREQUENCY * (value / 1024.0));
            }
        }
        else
        {
            counter++;
        }
        SampleBuffer buf = dac1.dequeue();

        for (size_t i = 0; i < buf.size(); i++)
        {
            buf[i] = SAMPLES_BUFFER[i];
        }

        dac1.write(buf);
    }
}
