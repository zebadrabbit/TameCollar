#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// ----------------------------
// Config (tuning knobs)
// ----------------------------
namespace Config {

// Serial
static constexpr uint32_t SerialBaud = 9600;

// Remote control input pins
static constexpr uint8_t RemotePin1 = 7;
static constexpr uint8_t RemotePin2 = 6;
static constexpr uint8_t RemotePin3 = 5;
static constexpr uint8_t RemotePin4 = 4;
static constexpr uint8_t RemotePinCount = 4;

static constexpr uint8_t RemoteIndexSolidUp = 0;   // REMOTE_PIN_1
static constexpr uint8_t RemoteIndexSolidDown = 1; // REMOTE_PIN_2
static constexpr uint8_t RemoteIndexAnimUp = 2;    // REMOTE_PIN_3
static constexpr uint8_t RemoteIndexAnimDown = 3;  // REMOTE_PIN_4

// WS2812 / NeoPixel
static constexpr uint8_t NeoPixelPin = A9;
static constexpr uint16_t PixelCount = 8;
static constexpr uint8_t StripBrightness = 30; // 0-255

// Power-saving loop (all modes except Danger)
static constexpr uint32_t SleepMs = 1000;
static constexpr uint32_t FadeMs = 250;
static constexpr uint8_t ActiveCycles = 2;
// Optional per-mode overrides for how many "cycles" to run each time we wake.
// (A cycle = one full animation pass that returns "cycle done").
static constexpr uint8_t ActiveCyclesPeace = 2;
static constexpr uint8_t ActiveCyclesWarning = 2;
static constexpr uint8_t ActiveCyclesSolid = 1;

// Solid color mode
static constexpr uint16_t SolidHoldMs = 3000;

// Peace mode
static constexpr uint16_t PeaceChaseStepMs = 90;
static constexpr uint16_t PeaceChaseSteps = 24;
static constexpr uint16_t PeaceHoldMs = 600;
static constexpr uint8_t PeaceBackgroundScale = 40; // 0-255 dim green base
static constexpr uint8_t PeaceChaseWidth = 2;

static constexpr uint16_t PeacePulseStepMs = 45;
static constexpr uint16_t PeacePulseSteps = 60;
static constexpr uint8_t PeacePulseMinScale = 30;  // 0-255
static constexpr uint8_t PeacePulseMaxScale = 200; // 0-255

static constexpr uint16_t PeaceSparkleStepMs = 70;
static constexpr uint16_t PeaceSparkleSteps = 28;
static constexpr uint8_t PeaceSparkleCount = 3;
static constexpr uint8_t PeaceSparkleScale = 255; // 0-255
static constexpr uint8_t PeaceSprinkleScale = 200; // 0-255 (brightness for blue/cyan/purple sprinkles)

// Warning mode (yellow hazard + white strobe)
static constexpr uint16_t WarningChaseStepMs = 80;
static constexpr uint16_t WarningChaseLaps = 5;
static constexpr uint8_t WarningChaseWidth = 3;
static constexpr uint16_t WarningStrobeStepMs = 60;
static constexpr uint16_t WarningStrobeSteps = 14;
static constexpr uint8_t WarningStrobeWhiteScale = 140; // 0-255 (lower = dimmer strobe)

// Danger mode
static constexpr uint16_t DangerChaseLaps = 5;
static constexpr uint8_t DangerChaseWidth = 2;
static constexpr uint16_t DangerChaseStepMs = 60;
static constexpr uint16_t DangerPulseStepMs = 50;
static constexpr uint16_t DangerPulseSteps = 60;
static constexpr uint8_t DangerFlashEvery = 5;
static constexpr uint8_t DangerPulseBase = 80;
static constexpr uint16_t DangerPulseTrianglePeriod = 20;
static constexpr uint16_t DangerCopStepMs = 140;
static constexpr uint16_t DangerCopSteps = 30;

// Common colors
static constexpr uint8_t ColorYellowR = 255;
static constexpr uint8_t ColorYellowG = 180;
static constexpr uint8_t ColorYellowB = 0;

static constexpr uint8_t ColorGreenR = 0;
static constexpr uint8_t ColorGreenG = 255;
static constexpr uint8_t ColorGreenB = 0;

static constexpr uint8_t ColorRedR = 255;
static constexpr uint8_t ColorRedG = 0;
static constexpr uint8_t ColorRedB = 0;

static constexpr uint8_t ColorBlueR = 0;
static constexpr uint8_t ColorBlueG = 0;
static constexpr uint8_t ColorBlueB = 255;

static constexpr uint8_t ColorCyanR = 0;
static constexpr uint8_t ColorCyanG = 255;
static constexpr uint8_t ColorCyanB = 255;

static constexpr uint8_t ColorPurpleR = 170;
static constexpr uint8_t ColorPurpleG = 0;
static constexpr uint8_t ColorPurpleB = 255;

static constexpr uint8_t ColorWhiteR = 255;
static constexpr uint8_t ColorWhiteG = 255;
static constexpr uint8_t ColorWhiteB = 255;

} // namespace Config


// Remote control input pins
static constexpr uint8_t REMOTE_PIN_1 = Config::RemotePin1;
static constexpr uint8_t REMOTE_PIN_2 = Config::RemotePin2;
static constexpr uint8_t REMOTE_PIN_3 = Config::RemotePin3;
static constexpr uint8_t REMOTE_PIN_4 = Config::RemotePin4; 

static constexpr uint8_t REMOTE_PIN_COUNT = Config::RemotePinCount;
static const uint8_t REMOTE_PINS[REMOTE_PIN_COUNT] = {REMOTE_PIN_1, REMOTE_PIN_2, REMOTE_PIN_3, REMOTE_PIN_4};
static const char *REMOTE_PIN_NAMES[REMOTE_PIN_COUNT] = {"REMOTE_PIN_1", "REMOTE_PIN_2", "REMOTE_PIN_3", "REMOTE_PIN_4"};

static bool lastRemoteStates[REMOTE_PIN_COUNT];
static bool remoteStatesInitialized = false;

static uint8_t remotePressEvents = 0;

static void printRemotePinState(uint8_t index, bool isHigh) {
  Serial.print(REMOTE_PIN_NAMES[index]);
  Serial.print(F(" (pin "));
  Serial.print(REMOTE_PINS[index]);
  Serial.print(F(") is "));
  Serial.println(isHigh ? F("HIGH") : F("LOW"));
}

static void initRemotePins() {
  for (uint8_t i = 0; i < REMOTE_PIN_COUNT; i++) {
    pinMode(REMOTE_PINS[i], INPUT_PULLUP);
  }

  for (uint8_t i = 0; i < REMOTE_PIN_COUNT; i++) {
    const bool isHigh = (digitalRead(REMOTE_PINS[i]) == HIGH);
    lastRemoteStates[i] = isHigh;
    printRemotePinState(i, isHigh);
  }

  remoteStatesInitialized = true;
}

static void pollRemotePinsForChanges() {
  if (!remoteStatesInitialized) {
    return;
  }

  for (uint8_t i = 0; i < REMOTE_PIN_COUNT; i++) {
    const bool isHigh = (digitalRead(REMOTE_PINS[i]) == HIGH);
    if (isHigh != lastRemoteStates[i]) {
      const bool wasHigh = lastRemoteStates[i];
      lastRemoteStates[i] = isHigh;
      printRemotePinState(i, isHigh);

      // With INPUT_PULLUP: pressed = LOW. Trigger actions on press (HIGH -> LOW).
      if (wasHigh && !isHigh) {
        remotePressEvents |= (uint8_t)(1u << i);
      }
    }
  }
}

// WS2812 / NeoPixel data pin
static constexpr uint8_t NEOPIXEL_PIN = Config::NeoPixelPin;
static constexpr uint16_t PIXEL_COUNT = Config::PixelCount;

static constexpr uint8_t STRIP_BRIGHTNESS = Config::StripBrightness; // 0-255

// Ring mapping convention (as requested):
// - LED 1 is the top-right
// - LEDs increase clockwise
// - LED 8 is the top-left
// In code, we use 0-based indices: LED1 -> index 0, LED8 -> index 7.


Adafruit_NeoPixel strip(PIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

enum class LedMode : uint8_t {
  Idle = 1,
  Peace = 2,
  Warning = 3,
  Danger = 4,
  SolidGreen = 5,
  SolidYellow = 6,
  SolidRed = 7,
};

static uint32_t scaleColor(Adafruit_NeoPixel &s, uint32_t color, uint8_t scale) {
  // Adafruit_NeoPixel::Color() packs colors as 0x00RRGGBB.
  const uint8_t r0 = (uint8_t)((color >> 16) & 0xFF);
  const uint8_t g0 = (uint8_t)((color >> 8) & 0xFF);
  const uint8_t b0 = (uint8_t)(color & 0xFF);

  const uint8_t r = (uint8_t)((uint16_t)r0 * scale / 255);
  const uint8_t g = (uint8_t)((uint16_t)g0 * scale / 255);
  const uint8_t b = (uint8_t)((uint16_t)b0 * scale / 255);
  return s.Color(r, g, b);
}

static uint8_t triangleWave8(uint16_t step, uint16_t periodSteps) {
  if (periodSteps < 2) {
    return 255;
  }

  const uint16_t p = step % periodSteps;
  const uint16_t half = periodSteps / 2;
  if (p <= half) {
    return (uint8_t)((uint32_t)p * 255 / half);
  }
  return (uint8_t)((uint32_t)(periodSteps - p) * 255 / half);
}

static uint32_t wheel(Adafruit_NeoPixel &s, uint8_t wheelPos) {
  wheelPos = (uint8_t)(255 - wheelPos);
  if (wheelPos < 85) {
    return s.Color(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  if (wheelPos < 170) {
    wheelPos = (uint8_t)(wheelPos - 85);
    return s.Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  wheelPos = (uint8_t)(wheelPos - 170);
  return s.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

class LedRingController {
public:
  explicit LedRingController(Adafruit_NeoPixel &s) : strip(s) {}

  void begin() {
    strip.begin();
    strip.setBrightness(STRIP_BRIGHTNESS);
    strip.clear();
    strip.show();
  }

  LedMode mode() const { return currentMode; }

  void setMode(LedMode newMode, bool forceRestart = false) {
    if (!forceRestart && newMode == currentMode) {
      return;
    }

    currentMode = newMode;
    restartAnimation();
    resetPowerCycle();
  }

  void restartAnimation() {
    phase = 0;
    step = 0;
    lastTickMs = 0;
    idleCleared = false;
  }

  void resetPowerCycle() {
    powerState = PowerState::Active;
    activeCyclesDone = 0;
    powerStateStartMs = 0;
    powerOffCleared = false;
    strip.setBrightness(baseBrightness);
  }

  void update(uint32_t nowMs) {
    // Danger stays on continuously (no power-saving loop).
    if (currentMode == LedMode::Danger) {
      strip.setBrightness(baseBrightness);
      updateDanger(nowMs);
      return;
    }

    // Idle is already off.
    if (currentMode == LedMode::Idle) {
      strip.setBrightness(baseBrightness);
      updateIdle();
      return;
    }

    if (powerStateStartMs == 0) {
      powerStateStartMs = nowMs;
    }

    switch (powerState) {
      case PowerState::Sleeping: {
        if (!powerOffCleared) {
          strip.setBrightness(baseBrightness);
          strip.clear();
          show();
          powerOffCleared = true;
        }

        if (nowMs - powerStateStartMs >= kSleepMs) {
          powerState = PowerState::Active;
          powerStateStartMs = nowMs;
          powerOffCleared = false;
          strip.setBrightness(baseBrightness);
          activeCyclesDone = 0;
          restartAnimation();
        }
        return;
      }

      case PowerState::FadingOut: {
        const uint32_t elapsed = nowMs - powerStateStartMs;
        if (elapsed >= kFadeMs) {
          strip.setBrightness(baseBrightness);
          strip.clear();
          show();

          powerState = PowerState::Sleeping;
          powerStateStartMs = nowMs;
          powerOffCleared = true;
          return;
        }

        const uint16_t remaining = (uint16_t)(kFadeMs - elapsed);
        const uint8_t b = (uint8_t)((uint32_t)baseBrightness * remaining / kFadeMs);
        strip.setBrightness(b);
        show();
        return;
      }

      case PowerState::Active:
      default: {
        strip.setBrightness(baseBrightness);

        bool cycleDone = false;
        switch (currentMode) {
          case LedMode::Peace:
            cycleDone = updatePeace(nowMs);
            break;
          case LedMode::Warning:
            cycleDone = updateWarning(nowMs);
            break;
          case LedMode::SolidGreen:
            cycleDone = updateSolidColor(nowMs, strip.Color(Config::ColorGreenR, Config::ColorGreenG, Config::ColorGreenB));
            break;
          case LedMode::SolidYellow:
            cycleDone = updateSolidColor(nowMs, strip.Color(Config::ColorYellowR, Config::ColorYellowG, Config::ColorYellowB));
            break;
          case LedMode::SolidRed:
            cycleDone = updateSolidColor(nowMs, strip.Color(Config::ColorRedR, Config::ColorRedG, Config::ColorRedB));
            break;
          default:
            // Should not happen (Idle/Danger handled above)
            break;
        }

        if (cycleDone) {
          activeCyclesDone++;
          if (activeCyclesDone >= activeCyclesTarget()) {
            powerState = PowerState::FadingOut;
            powerStateStartMs = nowMs;
          } else {
            restartAnimation();
          }
        }
        return;
      }
    }
  }

private:
  Adafruit_NeoPixel &strip;
  LedMode currentMode = LedMode::Idle;

  enum class PowerState : uint8_t {
    Active = 0,
    FadingOut = 1,
    Sleeping = 2,
  };

  static constexpr uint32_t kSleepMs = Config::SleepMs;
  static constexpr uint32_t kFadeMs = Config::FadeMs;
  static constexpr uint8_t baseBrightness = STRIP_BRIGHTNESS;

  uint8_t activeCyclesTarget() const {
    uint8_t target = Config::ActiveCycles;
    switch (currentMode) {
      case LedMode::Peace:
        target = Config::ActiveCyclesPeace;
        break;
      case LedMode::Warning:
        target = Config::ActiveCyclesWarning;
        break;
      case LedMode::SolidGreen:
      case LedMode::SolidYellow:
      case LedMode::SolidRed:
        target = Config::ActiveCyclesSolid;
        break;
      default:
        target = Config::ActiveCycles;
        break;
    }

    // Avoid a "0 cycles" configuration that would immediately fade out.
    return (target == 0) ? 1 : target;
  }

  PowerState powerState = PowerState::Active;
  uint8_t activeCyclesDone = 0;
  uint32_t powerStateStartMs = 0;
  bool powerOffCleared = false;

  // Shared animation state
  uint8_t phase = 0;
  uint16_t step = 0;
  uint32_t lastTickMs = 0;
  bool idleCleared = false;

  void show() { strip.show(); }

  void setAll(uint32_t color) {
    for (uint16_t i = 0; i < PIXEL_COUNT; i++) {
      strip.setPixelColor(i, color);
    }
  }

  void chaseSingle(uint32_t color, uint8_t pos) {
    strip.clear();
    strip.setPixelColor(pos % PIXEL_COUNT, color);
  }

  void chaseSegment(uint32_t color, uint8_t headPos, uint8_t width) {
    strip.clear();
    for (uint8_t w = 0; w < width; w++) {
      strip.setPixelColor((headPos + w) % PIXEL_COUNT, color);
    }
  }

  void updateIdle() {
    if (idleCleared) {
      return;
    }
    strip.clear();
    show();
    idleCleared = true;
  }

  bool updateSolidColor(uint32_t nowMs, uint32_t color) {
    // One cycle = show solid color briefly.
    if (phase == 0) {
      setAll(color);
      show();
      phase = 1;
      lastTickMs = nowMs;
      return false;
    }

    if (nowMs - lastTickMs < Config::SolidHoldMs) {
      return false;
    }

    return true;
  }

  // Mode 2: Peace - multiple phases, all green-focused.
  bool updatePeace(uint32_t nowMs) {
    const uint32_t green = strip.Color(Config::ColorGreenR, Config::ColorGreenG, Config::ColorGreenB);
    const uint32_t dimGreen = scaleColor(strip, green, Config::PeaceBackgroundScale);
    const uint32_t blue = strip.Color(Config::ColorBlueR, Config::ColorBlueG, Config::ColorBlueB);
    const uint32_t cyan = strip.Color(Config::ColorCyanR, Config::ColorCyanG, Config::ColorCyanB);
    const uint32_t purple = strip.Color(Config::ColorPurpleR, Config::ColorPurpleG, Config::ColorPurpleB);

    // phase 0: calm green chase over dim green background
    if (phase == 0) {
      if (nowMs - lastTickMs < Config::PeaceChaseStepMs) {
        return false;
      }
      lastTickMs = nowMs;

      const uint8_t pos = (uint8_t)(step % PIXEL_COUNT);
      setAll(dimGreen);
      for (uint8_t w = 0; w < Config::PeaceChaseWidth; w++) {
        strip.setPixelColor((pos + w) % PIXEL_COUNT, green);
      }
      show();

      step++;
      if (step >= Config::PeaceChaseSteps) {
        phase = 1;
        step = 0;
        lastTickMs = nowMs;
      }
      return false;
    }

    // phase 1: green "breathing" pulse
    if (phase == 1) {
      if (nowMs - lastTickMs < Config::PeacePulseStepMs) {
        return false;
      }
      lastTickMs = nowMs;

      const uint8_t wave = triangleWave8(step, Config::PeacePulseSteps);
      const uint16_t span = (uint16_t)(Config::PeacePulseMaxScale - Config::PeacePulseMinScale);
      const uint8_t intensity = (uint8_t)(Config::PeacePulseMinScale + (uint32_t)span * wave / 255);
      setAll(scaleColor(strip, green, intensity));
      show();

      step++;
      if (step >= Config::PeacePulseSteps) {
        phase = 2;
        step = 0;
        lastTickMs = nowMs;
      }
      return false;
    }

    // phase 2: soft sparkles (bright green points over dim green)
    if (phase == 2) {
      if (nowMs - lastTickMs < Config::PeaceSparkleStepMs) {
        return false;
      }
      lastTickMs = nowMs;

      setAll(dimGreen);
      const uint32_t sparkleGreen = scaleColor(strip, green, Config::PeaceSparkleScale);
      const uint32_t sprinkleBlue = scaleColor(strip, blue, Config::PeaceSprinkleScale);
      const uint32_t sprinkleCyan = scaleColor(strip, cyan, Config::PeaceSprinkleScale);
      const uint32_t sprinklePurple = scaleColor(strip, purple, Config::PeaceSprinkleScale);
      for (uint8_t j = 0; j < Config::PeaceSparkleCount; j++) {
        const uint8_t idx = (uint8_t)((step * 3u + j * 5u) % PIXEL_COUNT);
        // Mostly green sparkles, with occasional blue/cyan/purple "sprinkles".
        const uint8_t sel = (uint8_t)((step + j * 3u) % 12u);
        uint32_t c = sparkleGreen;
        if (sel == 0) {
          c = sprinkleCyan;
        } else if (sel == 1) {
          c = sprinklePurple;
        } else if (sel == 2) {
          c = sprinkleBlue;
        }
        strip.setPixelColor(idx, c);
      }
      show();

      step++;
      if (step >= Config::PeaceSparkleSteps) {
        phase = 3;
        step = 0;
        lastTickMs = nowMs;
      }
      return false;
    }

    // phase 3: brief solid green hold, then complete cycle
    if (phase == 3) {
      if (step == 0) {
        setAll(green);
        show();
        step = 1;
        lastTickMs = nowMs;
        return false;
      }

      if (nowMs - lastTickMs < Config::PeaceHoldMs) {
        return false;
      }

      return true;
    }

    return false;
  }

  // Mode 3: Warning (moved from former De-escalate)
  // Warning-y yellow hazard chase with occasional white strobes.
  bool updateWarning(uint32_t nowMs) {
    const uint32_t yellow = strip.Color(Config::ColorYellowR, Config::ColorYellowG, Config::ColorYellowB);
    const uint32_t white = strip.Color(Config::ColorWhiteR, Config::ColorWhiteG, Config::ColorWhiteB);

    // phase 0: yellow "hazard" chase around the ring
    if (phase == 0) {
      const uint16_t stepsTotal = (uint16_t)(Config::WarningChaseLaps * PIXEL_COUNT);
      if (nowMs - lastTickMs < Config::WarningChaseStepMs) {
        return false;
      }
      lastTickMs = nowMs;

      chaseSegment(yellow, (uint8_t)(step % PIXEL_COUNT), Config::WarningChaseWidth);
      show();

      step++;
      if (step >= stepsTotal) {
        phase = 1;
        step = 0;
        lastTickMs = nowMs;
      }
      return false;
    }

    // phase 1: brief white strobes alternating with yellow for drama
    if (phase == 1) {
      if (nowMs - lastTickMs < Config::WarningStrobeStepMs) {
        return false;
      }
      lastTickMs = nowMs;

      // "Police light" style: split the ring in half. One half is solid yellow,
      // the other half is the dimmed white strobe. Swap sides each step.
      const uint32_t dimWhite = scaleColor(strip, white, Config::WarningStrobeWhiteScale);
      const bool swap = (step % 2) == 1;
      for (uint16_t i = 0; i < PIXEL_COUNT; i++) {
        const bool firstHalf = i < (PIXEL_COUNT / 2);
        const bool whiteSide = (firstHalf ^ swap);
        strip.setPixelColor(i, whiteSide ? dimWhite : yellow);
      }
      show();

      step++;
      if (step >= Config::WarningStrobeSteps) {
        return true;
      }
      return false;
    }

    return true;
  }

  // Mode 4: red chase, then fast flash/pulse, then cop-lights (4 red, 4 blue) alternating.
  void updateDanger(uint32_t nowMs) {
    const uint32_t red = strip.Color(Config::ColorRedR, Config::ColorRedG, Config::ColorRedB);
    const uint32_t blue = strip.Color(Config::ColorBlueR, Config::ColorBlueG, Config::ColorBlueB);

    // phase 0: fast red chase
    if (phase == 0) {
      const uint16_t stepsTotal = (uint16_t)(Config::DangerChaseLaps * PIXEL_COUNT);
      if (nowMs - lastTickMs < Config::DangerChaseStepMs) {
        return;
      }
      lastTickMs = nowMs;

      chaseSegment(red, (uint8_t)(step % PIXEL_COUNT), Config::DangerChaseWidth);
      show();

      step++;
      if (step >= stepsTotal) {
        phase = 1;
        step = 0;
        lastTickMs = nowMs;
      }
      return;
    }

    // phase 1: flash/pulse red quickly
    if (phase == 1) {
      if (nowMs - lastTickMs < Config::DangerPulseStepMs) {
        return;
      }
      lastTickMs = nowMs;

      const bool flash = (step % Config::DangerFlashEvery) == 0;
      const uint8_t intensity =
          flash ? 255 : (uint8_t)(Config::DangerPulseBase + triangleWave8(step, Config::DangerPulseTrianglePeriod) / 2);
      setAll(scaleColor(strip, red, intensity));
      show();

      step++;
      if (step >= Config::DangerPulseSteps) {
        phase = 2;
        step = 0;
        lastTickMs = nowMs;
      }
      return;
    }

    // phase 2: "cop lights" split 4+4 alternating red/blue
    if (nowMs - lastTickMs < Config::DangerCopStepMs) {
      return;
    }
    lastTickMs = nowMs;

    const bool swap = (step % 2) == 1;
    for (uint16_t i = 0; i < PIXEL_COUNT; i++) {
      const bool firstHalf = i < (PIXEL_COUNT / 2);
      const uint32_t c = (firstHalf ^ swap) ? red : blue;
      strip.setPixelColor(i, c);
    }
    show();

    step++;
    if (step >= Config::DangerCopSteps) {
      phase = 0;
      step = 0;
      lastTickMs = nowMs;
    }
  }

  
};

static LedRingController ring(strip);

static void printMode(LedMode m) {
  switch (m) {
    case LedMode::Idle:
      Serial.println(F("Mode 1: Idle"));
      break;
    case LedMode::Peace:
      Serial.println(F("Mode 2: Peace"));
      break;
    case LedMode::Warning:
      Serial.println(F("Mode 3: Warning"));
      break;
    case LedMode::Danger:
      Serial.println(F("Mode 4: Danger"));
      break;
    case LedMode::SolidGreen:
      Serial.println(F("Solid: Green"));
      break;
    case LedMode::SolidYellow:
      Serial.println(F("Solid: Yellow"));
      break;
    case LedMode::SolidRed:
      Serial.println(F("Solid: Red"));
      break;
  }
}

static void pollSerialForModeChange() {
  if (!Serial.available()) {
    return;
  }

  const char c = (char)Serial.read();
  if (c == '1') {
    ring.setMode(LedMode::Idle);
    printMode(LedMode::Idle);
  } else if (c == '2') {
    ring.setMode(LedMode::Peace);
    printMode(LedMode::Peace);
  } else if (c == '3') {
    ring.setMode(LedMode::Warning);
    printMode(LedMode::Warning);
  } else if (c == '4') {
    ring.setMode(LedMode::Danger);
    printMode(LedMode::Danger);
  }
}

static LedMode modeStepUpForRemote(LedMode m) {
  // Per request: Idle -> Peace -> Warning -> Danger (clamp at Danger).
  switch (m) {
    case LedMode::Idle:
      return LedMode::Peace;
    case LedMode::Peace:
      return LedMode::Warning;
    case LedMode::Warning:
      return LedMode::Danger;
    case LedMode::Danger:
    default:
      return LedMode::Danger;
  }
}

static LedMode modeStepDownForRemote(LedMode m) {
  // Danger -> Warning -> Peace -> Idle (clamp at Idle).
  switch (m) {
    case LedMode::Danger:
      return LedMode::Warning;
    case LedMode::Warning:
      return LedMode::Peace;
    case LedMode::Peace:
    case LedMode::Idle:
    default:
      return LedMode::Idle;
  }
}

static LedMode modeStepUpSolidForRemote(LedMode m) {
  // Idle -> SolidGreen -> SolidYellow -> SolidRed (clamp at SolidRed).
  switch (m) {
    case LedMode::Idle:
      return LedMode::SolidGreen;
    case LedMode::SolidGreen:
      return LedMode::SolidYellow;
    case LedMode::SolidYellow:
      return LedMode::SolidRed;
    case LedMode::SolidRed:
      return LedMode::SolidRed;
    default:
      // If currently in an animated mode, start solid sequence at green.
      return LedMode::SolidGreen;
  }
}

static LedMode modeStepDownSolidForRemote(LedMode m) {
  // SolidRed -> SolidYellow -> SolidGreen -> Idle (clamp at Idle).
  switch (m) {
    case LedMode::SolidRed:
      return LedMode::SolidYellow;
    case LedMode::SolidYellow:
      return LedMode::SolidGreen;
    case LedMode::SolidGreen:
      return LedMode::Idle;
    case LedMode::Idle:
      return LedMode::Idle;
    default:
      // If currently in an animated mode, pressing "down" goes to Idle/off.
      return LedMode::Idle;
  }
}

void setup() {  
  Serial.begin(Config::SerialBaud);
  initRemotePins();
  ring.begin();
  ring.setMode(LedMode::Idle);
  Serial.println(F("LED ring ready. Send 1/2/3/4 over Serial to change modes."));
}

void loop() {
  pollSerialForModeChange();
  pollRemotePinsForChanges();

  const uint8_t events = remotePressEvents;
  if (events != 0) {
    remotePressEvents = 0;

    // Solid color modes
    if (events & (1u << Config::RemoteIndexSolidUp)) {
      const LedMode next = modeStepUpSolidForRemote(ring.mode());
      ring.setMode(next);
      printMode(next);
    }

    if (events & (1u << Config::RemoteIndexSolidDown)) {
      const LedMode next = modeStepDownSolidForRemote(ring.mode());
      ring.setMode(next, next == LedMode::Idle);
      printMode(next);
    }

    // Animated modes
    if (events & (1u << Config::RemoteIndexAnimUp)) {
      const LedMode next = modeStepUpForRemote(ring.mode());
      ring.setMode(next);
      printMode(next);
    }

    if (events & (1u << Config::RemoteIndexAnimDown)) {
      const LedMode current = ring.mode();
      if (current == LedMode::Idle) {
        ring.setMode(LedMode::Idle, true);
        printMode(LedMode::Idle);
      } else {
        const LedMode next = modeStepDownForRemote(current);
        ring.setMode(next);
        printMode(next);
      }
    }
  }

  ring.update(millis());
}