/*
  h725_core_map.h - Minimal grblHAL board map for the H725 Core V250919 module

  Part of grblHAL
*/

#if N_ABC_MOTORS > 0
#error "H725 Core map supports 3 motors max."
#endif

#if !defined(STM32H725xx) || HSE_VALUE != 24000000
#error "This board uses STM32H725xx with a 24MHz crystal, select the matching build."
#endif

#define BOARD_NAME "H725 Core V250919"

// Main PC communication UART on the core board.
#define SERIAL_PORT                 32      // GPIOD: TX = 8, RX = 9, USART3

// Define step pulse output pins.
#define X_STEP_PORT                 GPIOA
#define X_STEP_PIN                  8
#define Y_STEP_PORT                 GPIOB
#define Y_STEP_PIN                  2
#define Z_STEP_PORT                 GPIOB
#define Z_STEP_PIN                  5
#define STEP_OUTMODE                GPIO_SINGLE

// Define step direction output pins.
#define X_DIRECTION_PORT            GPIOA
#define X_DIRECTION_PIN             9
#define Y_DIRECTION_PORT            GPIOB
#define Y_DIRECTION_PIN             3
#define Z_DIRECTION_PORT            GPIOB
#define Z_DIRECTION_PIN             6
#define DIRECTION_OUTMODE           GPIO_SINGLE

#define STEPPERS_ENABLE_PORT    GPIOB
#define STEPPERS_ENABLE_PIN     8

// Define homing/hard limit switch input pins.
#define X_LIMIT_PORT                GPIOA
#define X_LIMIT_PIN                 10
#define Y_LIMIT_PORT                GPIOB
#define Y_LIMIT_PIN                 4
#define Z_LIMIT_PORT                GPIOB
#define Z_LIMIT_PIN                 7
#define LIMIT_INMODE                GPIO_SINGLE

#define AUXOUTPUT0_PORT             GPIOC   // Spindle PWM
#define AUXOUTPUT0_PIN              8
// #define AUXOUTPUT1_PORT             GPIOB   // Spindle direction
// #define AUXOUTPUT1_PIN              5
#define AUXOUTPUT2_PORT             GPIOC   // Spindle enable
#define AUXOUTPUT2_PIN              6
// // PD14/PD15 are wired to the onboard ESP32-C3 UART and must stay reserved.

#if DRIVER_SPINDLE_ENABLE & SPINDLE_ENA
#define SPINDLE_ENABLE_PORT         AUXOUTPUT2_PORT
#define SPINDLE_ENABLE_PIN          AUXOUTPUT2_PIN
#endif
#if DRIVER_SPINDLE_ENABLE & SPINDLE_PWM
#define SPINDLE_PWM_PORT            AUXOUTPUT0_PORT
#define SPINDLE_PWM_PIN             AUXOUTPUT0_PIN
#endif
// #if DRIVER_SPINDLE_ENABLE & SPINDLE_DIR
// #define SPINDLE_DIRECTION_PORT      AUXOUTPUT1_PORT
// #define SPINDLE_DIRECTION_PIN       AUXOUTPUT1_PIN
// #endif

// #undef COOLANT_ENABLE
// #define COOLANT_ENABLE              0

// #define AUXINPUT0_PORT              GPIOC   // Safety door
// #define AUXINPUT0_PIN               0
#define AUXINPUT1_PORT              GPIOB   // Probe
#define AUXINPUT1_PIN               10
#define AUXINPUT2_PORT              GPIOB   // Reset
#define AUXINPUT2_PIN               11
// #define AUXINPUT3_PORT              GPIOC   // Feed hold
// #define AUXINPUT3_PIN               5
// #define AUXINPUT4_PORT              GPIOC   // Cycle start
// #define AUXINPUT4_PIN               6


// // Additional exposed GPIOs on the board headers.
// #define AUXINPUT5_PORT              GPIOC
// #define AUXINPUT5_PIN               6
// #define AUXINPUT6_PORT              GPIOC
// #define AUXINPUT6_PIN               7
// #define AUXINPUT7_PORT              GPIOC
// #define AUXINPUT7_PIN               8
// #define AUXINPUT8_PORT              GPIOC
// #define AUXINPUT8_PIN               9
// #define AUXINPUT9_PORT              GPIOC
// #define AUXINPUT9_PIN               10
// #define AUXINPUT10_PORT             GPIOC
// #define AUXINPUT10_PIN              11
// #define AUXINPUT11_PORT             GPIOC
// #define AUXINPUT11_PIN              12

#if CONTROL_ENABLE & CONTROL_HALT
#define RESET_PORT                  AUXINPUT2_PORT
#define RESET_PIN                   AUXINPUT2_PIN
#endif
// #if CONTROL_ENABLE & CONTROL_FEED_HOLD
// #define FEED_HOLD_PORT              AUXINPUT3_PORT
// #define FEED_HOLD_PIN               AUXINPUT3_PIN
// #endif
// #if CONTROL_ENABLE & CONTROL_CYCLE_START
// #define CYCLE_START_PORT            AUXINPUT4_PORT
// #define CYCLE_START_PIN             AUXINPUT4_PIN
// #endif

// #if SAFETY_DOOR_ENABLE
// #define SAFETY_DOOR_PORT            AUXINPUT0_PORT
// #define SAFETY_DOOR_PIN             AUXINPUT0_PIN
// #endif

#if PROBE_ENABLE
#define PROBE_PORT                  AUXINPUT1_PORT
#define PROBE_PIN                   AUXINPUT1_PIN
#endif
