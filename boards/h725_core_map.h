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

// On-board status LED.
#define STATUS_LED_PORT              GPIOC
#define STATUS_LED_PIN               13
#define STATUS_LED_INVERT            0

// Main PC communication UART on the core board.
#define SERIAL_PORT                 32      // GPIOD: TX = 8, RX = 9, USART3

// 定义龙门式三轴步进电机的步进和方向引脚
#define X_STEP_PORT                 GPIOB
#define X_STEP_PIN                  8
#define Y_STEP_PORT                 GPIOB
#define Y_STEP_PIN                  7
#define Z_STEP_PORT                 GPIOB
#define Z_STEP_PIN                  6
#define STEP_OUTMODE                GPIO_SINGLE

// 定义步进方向输出引脚
#define X_DIRECTION_PORT            GPIOE
#define X_DIRECTION_PIN             5
#define Y_DIRECTION_PORT            GPIOE
#define Y_DIRECTION_PIN             4
#define Z_DIRECTION_PORT            GPIOE
#define Z_DIRECTION_PIN             2
#define DIRECTION_OUTMODE           GPIO_SINGLE

// 定义回原点/限位开关输入引脚
#define X_LIMIT_PORT                GPIOB
#define X_LIMIT_PIN                 9
#define Y_LIMIT_PORT                GPIOD
#define Y_LIMIT_PIN                 10
#define Z_LIMIT_PORT                GPIOD
#define Z_LIMIT_PIN                 11
#define LIMIT_INMODE                GPIO_SINGLE

// 定义步进电机使能引脚
#define STEPPERS_ENABLE_PORT    GPIOB
#define STEPPERS_ENABLE_PIN     5

// 定义翻面步进电机的步进和方向引脚，X2是翻面轴，连接到X轴的步进电机驱动器上，使用不同的引脚以避免与主轴冲突
// #define X2_STEP_PORT                GPIOA
// #define X2_STEP_PIN                 11
// #define X2_DIRECTION_PORT           GPIOA
// #define X2_DIRECTION_PIN            12
// #define X2_STEP_OUTMODE             GPIO_SINGLE
// #define X2_DIRECTION_OUTMODE        GPIO_SINGLE
// #define X2_ENABLE_PORT              STEPPERS_ENABLE_PORT
// #define X2_ENABLE_PIN               STEPPERS_ENABLE_PIN
// #define X2_LIMIT_PORT               GPIOA
// #define X2_LIMIT_PIN                13

// 定义气泵的pwm控制引脚
#define AIR_PUMP_PWM_PORT               GPIOC //AIR_PUMP_EN
#define AIR_PUMP_PWM_PIN                9
// 气泵控制输入为低电平启动，高电平停止。
#define AIR_PUMP_PWM_ACTIVE_LEVEL       0
#define AIR_PUMP_PWM_INACTIVE_LEVEL     1

// 定义气泵两个电磁阀的引脚
// 分出口阀和入口阀，
// 入口阀通电，出口阀下电，气泵是真空吸气模式
// 入口阀下电，出口阀通电，气泵是吹气模式
// 入口阀和出口阀不能同时通电，否则会烧掉气

#define PUMP_INLET_VALVE_PORT         GPIOB //AIR_VALVE4_HF_EN
#define PUMP_INLET_VALVE_PIN          10
#define PUMP_OUTLET_VALVE_PORT        GPIOB //AIR_VALVE5_HF_EN
#define PUMP_OUTLET_VALVE_PIN         11
// 电磁阀为高电平吸合，低电平释放。
#define PUMP_INLET_VALVE_ACTIVE_LEVEL   1
#define PUMP_INLET_VALVE_INACTIVE_LEVEL 0
#define PUMP_OUTLET_VALVE_ACTIVE_LEVEL  1
#define PUMP_OUTLET_VALVE_INACTIVE_LEVEL 0

// 定义主轴上的气动滑台控制引脚，有两个气动滑台，分别是一个银浆点胶头，一个锡膏点胶头
// 单电控二位五通电磁阀控制气动滑台的工作，通电时，PA通，断电时PB通，控制气动滑台的伸出和收回
// 普通GPIO即可，不需要使用定时器的PWM输出模式，因为气动滑台的控制频率很低，几Hz就足够了
#define AIRSLIDE1_PORT              GPIOB //AIR_VALVE0_EN
#define AIRSLIDE1_PIN               1
#define AIRSLIDE2_PORT              GPIOB //AIR_VALVE1_EN
#define AIRSLIDE2_PIN               2
#define AIRSLIDE1_ACTIVE_LEVEL      1
#define AIRSLIDE1_INACTIVE_LEVEL    0
#define AIRSLIDE2_ACTIVE_LEVEL      1
#define AIRSLIDE2_INACTIVE_LEVEL    0

// 定义主轴上的气动夹具控制引脚，有点胶头，分别是一个银浆点胶头，一个锡膏点胶头
// 每个点胶头都由一个三通电磁阀控制，三通电磁阀通电时，控制点胶头输出，断电时，控制点胶头停止输出
// 普通GPIO即可，不需要使用定时器的PWM输出模式，因为点胶头的控制频率很低，几Hz就足够了
#define SILVER_PASTE_CLAMP_PORT          GPIOB //AIR_VALVE6_HF_EN
#define SILVER_PASTE_CLAMP_PIN           12 
#define SOLDER_PASTE_CLAMP_PORT          GPIOB //AIR_VALVE7_HF_EN
#define SOLDER_PASTE_CLAMP_PIN           13 
#define SILVER_PASTE_CLAMP_ACTIVE_LEVEL   1
#define SILVER_PASTE_CLAMP_INACTIVE_LEVEL 0
#define SOLDER_PASTE_CLAMP_ACTIVE_LEVEL   1
#define SOLDER_PASTE_CLAMP_INACTIVE_LEVEL 0
#define SILVER_PASTE_CLAMP_PULSE_MS       50U
#define SOLDER_PASTE_CLAMP_PULSE_MS       50U
// #define solder_paste_clamp_port          GPIOC
// #define solder_paste_clamp_pin           8

// 定义主轴上的光固化阻焊剂的控制引脚，有一个三通电磁阀控制光固化阻焊剂的输出，三通电磁阀通电时，控制光固化阻焊剂输出，断电时，控制光固化阻焊剂停止输出
// 普通GPIO即可，不需要使用定时器的PWM输出模式，因为光固化阻焊剂的控制频率很低，几Hz就足够了                GPIOC //AIR_VALVE8_HF_EN
#define UV_LAMP_PORT                    GPIOB //AIR_VALVE8_HF_EN
#define UV_LAMP_PIN                     14
#define UV_LAMP_ACTIVE_LEVEL            1
#define UV_LAMP_INACTIVE_LEVEL          0
#define UV_LAMP_PULSE_MS                1000U

// 定义主轴上的激光器控制引脚，pwm信号，控制激光器使能和功率
#define LASER_PWM_PORT                 GPIOC
#define LASER_PWM_PIN                  12
#define LASER_ENABLE_PORT              GPIOE  //临时挂到AIR_VALVE2_EN上，后续可以改成专用的引脚
#define LASER_ENABLE_PIN               7

// 定义主轴上的贴片吸嘴控制引脚，一个三通阀控制吸嘴工作，两个步进电机控制吸嘴的旋转和升降
// 三通阀通电时，吸嘴工作，断电时，吸嘴停止工作
#define SUCTION_NOZZLE_VALVE_PORT       GPIOE //AIR_VALVE3_EN
#define SUCTION_NOZZLE_VALVE_PIN        8
#define SUCTION_NOZZLE_VALVE_ACTIVE_LEVEL   1
#define SUCTION_NOZZLE_VALVE_INACTIVE_LEVEL 0

// 旋转电机控制引脚，步进电机控制吸嘴的旋转，普通GPIO即可，不需要使用定时器的PWM输出模式，因为旋转电机的控制频率很低，几Hz就足够了
// #define SUCTION_NOZZLE_ROTATE_STEP_PORT GPIOA 
// #define SUCTION_NOZZLE_ROTATE_STEP_PIN  11
// #define SUCTION_NOZZLE_ROTATE_DIRECTION_PORT GPIOA
// #define SUCTION_NOZZLE_ROTATE_DIRECTION_PIN 12

// 升降电机控制引脚，步进电机控制吸嘴的升降，普通GPIO即可，不需要使用定时器的PWM输出模式，因为升降电机的控制频率很低，几Hz就足够了
// #define SUCTION_NOZZLE_LIFT_STEP_PORT   GPIOA
// #define SUCTION_NOZZLE_LIFT_STEP_PIN    10
// #define SUCTION_NOZZLE_LIFT_DIRECTION_PORT GPIOA
// #define SUCTION_NOZZLE_LIFT_DIRECTION_PIN 13

// 定义玻璃板压平电机控制引脚，使用无刷电机控制
// #define GLASS_PLATE_FLATTEN_Direction_PORT GPIOA
// #define GLASS_PLATE_FLATTEN_Direction_PIN 9
// #define GLASS_PLATE_FLATTEN_PWM_PORT GPIOA
// #define GLASS_PLATE_FLATTEN_PWM_PIN 8

// 定义薄膜滚轴电机控制引脚，使用无刷电机控制
// 有两个电机，分别控制薄膜的进给和收卷
// #define FILM_ROLLER_FEED_DIRECTION_PORT GPIOA
// #define FILM_ROLLER_FEED_DIRECTION_PIN 9
// #define FILM_ROLLER_FEED_PWM_PORT GPIOA
// #define FILM_ROLLER_FEED_PWM_PIN 8
// #define FILM_ROLLER_REWIND_DIRECTION_PORT GPIOA
// #define FILM_ROLLER_REWIND_DIRECTION_PIN 9
// #define FILM_ROLLER_REWIND_PWM_PORT GPIOA
// #define FILM_ROLLER_REWIND_PWM_PIN 8

// 定义主轴上的钻孔电机控制引脚


// 定义气泵控制引脚


// 定义UV灯控制引脚


//定义超声波控制



#define AUXOUTPUT0_PORT             LASER_ENABLE_PORT   // Spindle PWM
#define AUXOUTPUT0_PIN              LASER_ENABLE_PIN
#define AUXOUTPUT1_PORT             LASER_PWM_PORT   // Spindle direction
#define AUXOUTPUT1_PIN              LASER_PWM_PIN
#define AUXOUTPUT2_PORT             PUMP_INLET_VALVE_PORT   
#define AUXOUTPUT2_PIN              PUMP_INLET_VALVE_PIN
#define AUXOUTPUT3_PORT             PUMP_OUTLET_VALVE_PORT
#define AUXOUTPUT3_PIN              PUMP_OUTLET_VALVE_PIN
#define AUXOUTPUT4_PORT             AIR_PUMP_PWM_PORT
#define AUXOUTPUT4_PIN              AIR_PUMP_PWM_PIN
#define AUXOUTPUT5_PORT             AIRSLIDE1_PORT
#define AUXOUTPUT5_PIN              AIRSLIDE1_PIN
#define AUXOUTPUT6_PORT             AIRSLIDE2_PORT
#define AUXOUTPUT6_PIN              AIRSLIDE2_PIN
#define AUXOUTPUT7_PORT             SILVER_PASTE_CLAMP_PORT
#define AUXOUTPUT7_PIN              SILVER_PASTE_CLAMP_PIN
#define AUXOUTPUT8_PORT             SOLDER_PASTE_CLAMP_PORT
#define AUXOUTPUT8_PIN              SOLDER_PASTE_CLAMP_PIN
#define AUXOUTPUT9_PORT             SUCTION_NOZZLE_VALVE_PORT
#define AUXOUTPUT9_PIN              SUCTION_NOZZLE_VALVE_PIN
#define AUXOUTPUT10_PORT            UV_LAMP_PORT
#define AUXOUTPUT10_PIN             UV_LAMP_PIN

// // PD14/PD15 are wired to the onboard ESP32-C3 UART and must stay reserved.

#if DRIVER_SPINDLE_ENABLE & SPINDLE_ENA
#define SPINDLE_ENABLE_PORT         AUXOUTPUT0_PORT
#define SPINDLE_ENABLE_PIN          AUXOUTPUT0_PIN
#endif
#if DRIVER_SPINDLE_ENABLE & SPINDLE_PWM
#define SPINDLE_PWM_PORT            AUXOUTPUT1_PORT
#define SPINDLE_PWM_PIN             AUXOUTPUT1_PIN
#endif

// #undef COOLANT_ENABLE
// #define COOLANT_ENABLE              0

// #define AUXINPUT0_PORT              GPIOC   // Safety door
// #define AUXINPUT0_PIN               0
#define AUXINPUT1_PORT              GPIOD   // Probe
#define AUXINPUT1_PIN               2
#define AUXINPUT2_PORT              GPIOA   // Reset
#define AUXINPUT2_PIN               5
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
