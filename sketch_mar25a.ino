#include <M5Unified.h>
#include <sys/time.h>

void setup(void) {
  Serial.begin(115200);
  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Display.setEpdMode(epd_mode_t::epd_fast);

  M5.Display.setRotation(1);
  M5.Display.setTextFont(&fonts::Orbitron_Light_32);

  M5.Rtc.setDateTime({ { 0, 0, 0 }, { 0, 0, 0 } });
}

void loop(void) {
  M5.update();

  static struct timeval tv1;
  static struct timeval tv2;

  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    if (incomingByte == 'x') {
      M5.Display.fillRect(0, 0, 960, 540, TFT_WHITE);
      M5.Display.setCursor(0, 0);
    }
    if (incomingByte == 's') {
      gettimeofday(&tv1, NULL);
    }
    if (incomingByte == 'f') {
      gettimeofday(&tv2, NULL);
      long long time1 = (long long)tv2.tv_sec * 1000 + tv2.tv_usec / 1000;
      long long time2 = (long long)tv1.tv_sec * 1000 + tv1.tv_usec / 1000;
      long long diff = time1 - time2;
      long long sec = diff / 1000;
      long long milli = diff % 1000;
      M5.Display.printf("Timp : %lld secunde %lld milisecunde\r\n", sec, milli);
    }
  }
}