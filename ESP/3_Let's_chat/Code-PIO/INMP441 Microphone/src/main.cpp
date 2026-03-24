#include <Arduino.h>
// #include <driver/i2s.h>

// // you shouldn't need to change these settings
#define SAMPLE_BUFFER_SIZE 512
// #define SAMPLE_RATE 8000
// // most microphones will probably default to left channel but you may need to tie the L/R pin low
// #define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
// // either wire your microphone to the same pins or change these to match your wiring
// #define I2S_MIC_SERIAL_CLOCK GPIO_NUM_33
// #define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_25
// #define I2S_MIC_SERIAL_DATA GPIO_NUM_32

// // don't mess around with this
// i2s_config_t i2s_config = {
//     .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
//     .sample_rate = SAMPLE_RATE,
//     .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
//     .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
//     .communication_format = I2S_COMM_FORMAT_STAND_I2S,
//     .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
//     .dma_buf_count = 4,
//     .dma_buf_len = 1024,
//     .use_apll = false,
//     .tx_desc_auto_clear = false,
//     .fixed_mclk = 0};

// // and don't mess around with this
// i2s_pin_config_t i2s_mic_pins = {
//     .bck_io_num = I2S_MIC_SERIAL_CLOCK,
//     .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
//     .data_out_num = I2S_PIN_NO_CHANGE,
//     .data_in_num = I2S_MIC_SERIAL_DATA};

// void setup()
// {
//   delay(1000); // give us a moment to open the serial plotter before we start sending data
//   // we need serial output for the plotter
//   Serial.begin(115200);
//   // start up the I2S peripheral
//   i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
//   i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);
//   delay(500); // give the I2S peripheral a moment to start up
// }

// int32_t raw_samples[SAMPLE_BUFFER_SIZE];
// void loop()
// {
//   // read from the I2S device
//   // size_t bytes_read = 0;
//   // i2s_read(I2S_NUM_0, &raw_samples, sizeof(int32_t) * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
//   // int samples_read = bytes_read / sizeof(int32_t);
//   // // dump the samples out to the serial channel.
//   // for (int i = 0; i < samples_read; i++)
//   // {
//   //     Serial.printf("%ld\n", raw_samples[i]);
//   // }
//   int32_t sample = 0;
//   int bytes = i2s_pop_sample(I2S_NUM_0, (char*)&sample, portMAX_DELAY);
//   if(bytes > 0){
//     Serial.println(sample);
//   }
// }
#include <driver/i2s.h>
// #define I2S_WS 15
// #define I2S_SD 13
// #define I2S_SCK 2

// #define I2S_MIC_SERIAL_CLOCK GPIO_NUM_33
// #define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_25
// #define I2S_MIC_SERIAL_DATA GPIO_NUM_32
#define I2S_WS 25
#define I2S_SD 32
#define I2S_SCK 33
#define I2S_PORT I2S_NUM_0
#define bufferLen 64
int16_t sBuffer[bufferLen];

void i2s_install();
void i2s_setpin();

void setup() {
  Serial.begin(115200);
  Serial.println("Setup I2S ...");

  delay(1000);
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
  delay(500);
}

void loop() {
  size_t bytesIn = 0;
  esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
  if (result == ESP_OK)
  {
    int samples_read = bytesIn / 8;
    if (samples_read > 0) {
      float mean = 0;
      for (int i = 0; i < samples_read; ++i) {
        mean += (sBuffer[i]);
      }
      mean /= samples_read;
      Serial.println(mean);
    }
  }
}

void i2s_install(){
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin(){
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}