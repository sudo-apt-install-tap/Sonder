#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <vector>
#include <algorithm>
#include <Audio.h>
#include <U8g2lib.h>

// I2S -> PCM5102
constexpr int PIN_I2S_BCK  = 26;  // BCLK
constexpr int PIN_I2S_LRC  = 25;  // LCK
constexpr int PIN_I2S_DOUT = 27;  // DIN

// SD card (SPI)
constexpr int PIN_SD_SCK  = 18;
constexpr int PIN_SD_MISO = 19;
constexpr int PIN_SD_MOSI = 23;
constexpr int PIN_SD_CS   = 13;

// OLED (I2C)
constexpr int PIN_OLED_SDA = 21;
constexpr int PIN_OLED_SCL = 22;

// Buttons (SW1..SW4, each wired to GND)
constexpr int PIN_BTN_PREV = 33;  // SW1 "Previous"
constexpr int PIN_BTN_PLAY = 32;  // SW2 "Play/Pause"
constexpr int PIN_BTN_NEXT = 16;  // SW3 "Next"
constexpr int PIN_BTN_MODE = 17;  // SW4 "Menu"

// Rotary encoder
constexpr int PIN_ENC_A  = 4;
constexpr int PIN_ENC_B  = 5;
constexpr int PIN_ENC_SW = 14;
// =========================================================================

constexpr int VOLUME_MAX = 21;  // ESP32-audioI2S volume range is 0..21

SPIClass sdSPI(VSPI);
Audio audio;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE,
                                            PIN_OLED_SCL, PIN_OLED_SDA);

std::vector<String> tracks;
int  current = 0;
int  volume = 8;
bool paused = false;
bool stopped = false;
bool shuffle = false;
bool muted = false;
bool dirty = true;
uint32_t trackStartMs = 0;
uint32_t lastDrawMs = 0;
int failCount = 0;

// ---------------------------- buttons ----------------------------------
struct Button {
  uint8_t pin;
  bool last;
  uint32_t t;
};

Button btnPrev{PIN_BTN_PREV, HIGH, 0};
Button btnPlay{PIN_BTN_PLAY, HIGH, 0};
Button btnNext{PIN_BTN_NEXT, HIGH, 0};
Button btnMode{PIN_BTN_MODE, HIGH, 0};
Button btnEnc {PIN_ENC_SW,   HIGH, 0};

bool pressed(Button &b) {
  bool now = digitalRead(b.pin);
  if (now != b.last && millis() - b.t > 30) {
    b.t = millis();
    b.last = now;
    if (now == LOW) return true;
  }
  return false;
}


int8_t readEncoder() {
  static uint8_t prev = 0;
  static int8_t acc = 0;
  static const int8_t table[16] = {0, -1, 1, 0, 1, 0, 0, -1,
                                   -1, 0, 0, 1, 0, 1, -1, 0};
  uint8_t cur = (digitalRead(PIN_ENC_A) << 1) | digitalRead(PIN_ENC_B);
  prev = ((prev << 2) | cur) & 0x0F;
  acc += table[prev];
  if (acc >= 4)  { acc = 0; return 1; }
  if (acc <= -4) { acc = 0; return -1; }
  return 0;
}

// ---------------------------- display ----------------------------------
void draw() {
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);
  char line[32];

  if (tracks.empty()) {
    display.drawStr(0, 12, "No tracks on SD");
  } else {
    snprintf(line, sizeof(line), "%d/%d %s%s", current + 1, (int)tracks.size(),
             shuffle ? "SHF " : "", muted ? "MUTE" : "");
    display.drawStr(0, 10, line);
    display.drawStr(0, 26, tracks[current].substring(0, 21).c_str());
    display.drawStr(0, 42, stopped ? "STOPPED" : (paused ? "PAUSED" : "PLAYING"));
  }

  int level = muted ? 0 : volume;
  display.drawFrame(0, 54, 100, 8);
  display.drawBox(0, 54, (level * 100) / VOLUME_MAX, 8);
  display.sendBuffer();

  dirty = false;
  lastDrawMs = millis();
}

void showMessage(const char *msg) {
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(0, 12, msg);
  display.sendBuffer();
}

// ---------------------------- SD / tracks ------------------------------
void scanTracks() {
  File root = SD.open("/");
  if (!root) return;
  while (true) {
    File f = root.openNextFile();
    if (!f) break;
    String name = f.name();
    bool isDir = f.isDirectory();
    f.close();

    int slash = name.lastIndexOf('/');
    if (slash >= 0) name = name.substring(slash + 1);
    if (isDir || name.startsWith(".")) continue;  // skips hidden/._ files

    String lower = name;
    lower.toLowerCase();
    if (lower.endsWith(".mp3") || lower.endsWith(".wav") || lower.endsWith(".flac")) {
      tracks.push_back(name);
    }
  }
  root.close();
  std::sort(tracks.begin(), tracks.end(),
            [](const String &a, const String &b) { return a.compareTo(b) < 0; });
}

// ---------------------------- playback ---------------------------------
void playTrack(int index) {
  if (tracks.empty()) return;
  int n = tracks.size();
  current = ((index % n) + n) % n;
  String path = "/" + tracks[current];


  if (millis() - trackStartMs < 1000) failCount++; else failCount = 0;
  if (failCount >= n) {
    stopped = true;
    dirty = true;
    return;
  }

  audio.connecttoFS(SD, path.c_str());
  trackStartMs = millis();
  paused = false;
  stopped = false;
  dirty = true;
}

void nextTrack() {
  int n = tracks.size();
  if (n == 0) return;
  if (shuffle && n > 1) {
    int r;
    do { r = esp_random() % n; } while (r == current);
    playTrack(r);
  } else {
    playTrack(current + 1);
  }
}

void prevTrack() { playTrack(current - 1); }

void togglePause() {
  if (stopped) { playTrack(current); return; }
  audio.pauseResume();
  paused = !paused;
  dirty = true;
}

void applyVolume() {
  audio.setVolume(muted ? 0 : volume);
  dirty = true;
}

// ----------------------------- setup / loop ----------------------------
void setup() {
  Serial.begin(115200);

  pinMode(PIN_BTN_PREV, INPUT_PULLUP);
  pinMode(PIN_BTN_PLAY, INPUT_PULLUP);
  pinMode(PIN_BTN_NEXT, INPUT_PULLUP);
  pinMode(PIN_BTN_MODE, INPUT_PULLUP);
  pinMode(PIN_ENC_A, INPUT_PULLUP);
  pinMode(PIN_ENC_B, INPUT_PULLUP);
  pinMode(PIN_ENC_SW, INPUT_PULLUP);

  display.begin();
  showMessage("Booting...");

  sdSPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
  if (!SD.begin(PIN_SD_CS, sdSPI)) {
    showMessage("SD init failed");
    while (true) delay(1000);
  }
  scanTracks();

  audio.setPinout(PIN_I2S_BCK, PIN_I2S_LRC, PIN_I2S_DOUT);
  audio.setVolume(volume);

  if (!tracks.empty()) playTrack(0);
  dirty = true;
}

void loop() {
  audio.loop();

  if (pressed(btnPlay)) togglePause();
  if (pressed(btnNext)) nextTrack();
  if (pressed(btnPrev)) prevTrack();
  if (pressed(btnMode)) { shuffle = !shuffle; dirty = true; }
  if (pressed(btnEnc))  { muted = !muted; applyVolume(); }

  int8_t step = readEncoder();
  if (step != 0) {
    volume = constrain(volume + step, 0, VOLUME_MAX);
    muted = false;
    applyVolume();
  }

  if (!tracks.empty() && !paused && !stopped && !audio.isRunning() &&
      millis() - trackStartMs > 300) {
    nextTrack();
  }

  if (dirty && millis() - lastDrawMs > 100) draw();
}
