#include <Arduino_LSM9DS1.h>              // IMU
#include <deeplearning_inferencing.h>     // Edge Impulse 模型库

#define AXIS_COUNT        EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME
#define FEATURE_SIZE      EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE
float features[FEATURE_SIZE];
int sampleIndex = 0;
  
void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("❌ Failed to initialize IMU!");
    while (1);
  }

  Serial.println("✅ IMU ready, collecting data...");
}

void loop() {
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    float ax, ay, az, gx, gy, gz;
    IMU.readAcceleration(ax, ay, az);
    IMU.readGyroscope(gx, gy, gz);

    features[sampleIndex++] = ax;
    features[sampleIndex++] = ay;
    features[sampleIndex++] = az;
    features[sampleIndex++] = gx;
    features[sampleIndex++] = gy;
    features[sampleIndex++] = gz;
  }

  if (sampleIndex >= FEATURE_SIZE) {
    ei::signal_t signal;
    if (numpy::signal_from_buffer(features, FEATURE_SIZE, &signal) != 0) {
      Serial.println("❌ Signal error");
      sampleIndex = 0;
      return;
    }

    ei_impulse_result_t result;
    if (run_classifier(&signal, &result, false) != EI_IMPULSE_OK) {
      Serial.println("❌ Classifier failed");
      sampleIndex = 0;
      return;
    }

    // 找出最大值的标签
    float max_val = 0.0;
    int max_index = -1;
    for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
      if (result.classification[i].value > max_val) {
        max_val = result.classification[i].value;
        max_index = i;
      }
    }

    // 只打印最大概率那一行
    if (max_index >= 0) {
      Serial.print(result.classification[max_index].label);
      Serial.print(": ");
      Serial.println(result.classification[max_index].value, 3);
    }

    sampleIndex = 0;
    delay(200); // 限制频率
  }
}
