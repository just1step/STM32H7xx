#include "driver.h"

#include <string.h>

#include "grbl/core_handlers.h"
#include "grbl/report.h"
#include "grbl/task.h"

#if defined(BOARD_H725_CORE) && defined(PUMP_INLET_VALVE_PORT) && defined(PUMP_OUTLET_VALVE_PORT) && defined(AIR_PUMP_PWM_PORT)

#ifndef AIR_PUMP_PWM_ACTIVE_LEVEL
#define AIR_PUMP_PWM_ACTIVE_LEVEL 1
#endif
#ifndef AIR_PUMP_PWM_INACTIVE_LEVEL
#define AIR_PUMP_PWM_INACTIVE_LEVEL 0
#endif
#ifndef PUMP_INLET_VALVE_ACTIVE_LEVEL
#define PUMP_INLET_VALVE_ACTIVE_LEVEL 1
#endif
#ifndef PUMP_INLET_VALVE_INACTIVE_LEVEL
#define PUMP_INLET_VALVE_INACTIVE_LEVEL 0
#endif
#ifndef PUMP_OUTLET_VALVE_ACTIVE_LEVEL
#define PUMP_OUTLET_VALVE_ACTIVE_LEVEL 1
#endif
#ifndef PUMP_OUTLET_VALVE_INACTIVE_LEVEL
#define PUMP_OUTLET_VALVE_INACTIVE_LEVEL 0
#endif
#if defined(SILVER_PASTE_CLAMP_PORT)
#ifndef SILVER_PASTE_CLAMP_ACTIVE_LEVEL
#define SILVER_PASTE_CLAMP_ACTIVE_LEVEL 1
#endif
#ifndef SILVER_PASTE_CLAMP_INACTIVE_LEVEL
#define SILVER_PASTE_CLAMP_INACTIVE_LEVEL 0
#endif
#ifndef SILVER_PASTE_CLAMP_PULSE_MS
#define SILVER_PASTE_CLAMP_PULSE_MS 50U
#endif
#endif
#if defined(SOLDER_PASTE_CLAMP_PORT)
#ifndef SOLDER_PASTE_CLAMP_ACTIVE_LEVEL
#define SOLDER_PASTE_CLAMP_ACTIVE_LEVEL 1
#endif
#ifndef SOLDER_PASTE_CLAMP_INACTIVE_LEVEL
#define SOLDER_PASTE_CLAMP_INACTIVE_LEVEL 0
#endif
#ifndef SOLDER_PASTE_CLAMP_PULSE_MS
#define SOLDER_PASTE_CLAMP_PULSE_MS 50U
#endif
#endif
#if defined(AIRSLIDE1_PORT)
#ifndef AIRSLIDE1_ACTIVE_LEVEL
#define AIRSLIDE1_ACTIVE_LEVEL 1
#endif
#ifndef AIRSLIDE1_INACTIVE_LEVEL
#define AIRSLIDE1_INACTIVE_LEVEL 0
#endif
#endif

typedef enum {
    PumpState_Off = 0,
    PumpState_Pressure,
    PumpState_Vacuum
} pump_state_t;

static user_mcode_ptrs_t user_mcode;
static driver_setup_ptr driver_setup;
static driver_reset_ptr driver_reset;
static on_report_options_ptr on_report_options;
static pump_state_t pump_state = PumpState_Off;
#if defined(AIRSLIDE1_PORT)
static bool airslide1_on = false;
#endif

static inline void pump_write_level (GPIO_TypeDef *gpio, uint32_t pin, bool level)
{
    DIGITAL_OUT(gpio, 1UL << pin, level);
}

static inline void pump_set_air_pump (bool on)
{
    pump_write_level(AIR_PUMP_PWM_PORT, AIR_PUMP_PWM_PIN, on ? AIR_PUMP_PWM_ACTIVE_LEVEL : AIR_PUMP_PWM_INACTIVE_LEVEL);
}

static inline void pump_set_inlet_valve (bool on)
{
    pump_write_level(PUMP_INLET_VALVE_PORT, PUMP_INLET_VALVE_PIN, on ? PUMP_INLET_VALVE_ACTIVE_LEVEL : PUMP_INLET_VALVE_INACTIVE_LEVEL);
}

static inline void pump_set_outlet_valve (bool on)
{
    pump_write_level(PUMP_OUTLET_VALVE_PORT, PUMP_OUTLET_VALVE_PIN, on ? PUMP_OUTLET_VALVE_ACTIVE_LEVEL : PUMP_OUTLET_VALVE_INACTIVE_LEVEL);
}

#if defined(SILVER_PASTE_CLAMP_PORT)
static inline void pump_set_silver_paste_clamp (bool on)
{
    pump_write_level(SILVER_PASTE_CLAMP_PORT, SILVER_PASTE_CLAMP_PIN, on ? SILVER_PASTE_CLAMP_ACTIVE_LEVEL : SILVER_PASTE_CLAMP_INACTIVE_LEVEL);
}

static void silver_paste_clamp_off (void *data)
{
    UNUSED(data);
    pump_set_silver_paste_clamp(false);
}
#endif

#if defined(SOLDER_PASTE_CLAMP_PORT)
static inline void pump_set_solder_paste_clamp (bool on)
{
    pump_write_level(SOLDER_PASTE_CLAMP_PORT, SOLDER_PASTE_CLAMP_PIN, on ? SOLDER_PASTE_CLAMP_ACTIVE_LEVEL : SOLDER_PASTE_CLAMP_INACTIVE_LEVEL);
}

static void solder_paste_clamp_off (void *data)
{
    UNUSED(data);
    pump_set_solder_paste_clamp(false);
}
#endif

#if defined(AIRSLIDE1_PORT)
static inline void pump_set_airslide1 (bool on)
{
    pump_write_level(AIRSLIDE1_PORT, AIRSLIDE1_PIN, on ? AIRSLIDE1_ACTIVE_LEVEL : AIRSLIDE1_INACTIVE_LEVEL);
    airslide1_on = on;
}
#endif

static bool pump_trigger_pulse (foreground_task_ptr off_task, GPIO_TypeDef *gpio, uint32_t pin, bool active_level, uint32_t pulse_ms)
{
    task_delete(off_task, NULL);
    DIGITAL_OUT(gpio, 1UL << pin, active_level);

    if(task_add_delayed(off_task, NULL, pulse_ms))
        return true;

    DIGITAL_OUT(gpio, 1UL << pin, !active_level);

    return false;
}

static void pump_apply_state (pump_state_t state)
{
    switch(state) {

        case PumpState_Pressure:
            // Keep valve outputs mutually exclusive during the transition.
            pump_set_inlet_valve(false);
            pump_set_outlet_valve(true);
            pump_set_air_pump(true);
            break;

        case PumpState_Vacuum:
            pump_set_outlet_valve(false);
            pump_set_inlet_valve(true);
            pump_set_air_pump(true);
            break;

        case PumpState_Off:
        default:
            pump_set_air_pump(false);
            pump_set_inlet_valve(false);
            pump_set_outlet_valve(false);
            state = PumpState_Off;
            break;
    }

    pump_state = state;
}

static user_mcode_type_t pump_mcode_check (user_mcode_t mcode)
{
    return mcode == UserMCode_Generic1 || mcode == UserMCode_Generic2 || mcode == UserMCode_Generic3
#if defined(AIRSLIDE1_PORT)
        || mcode == UserMCode_Generic4 || mcode == UserMCode_Generic5
#endif
#if defined(SILVER_PASTE_CLAMP_PORT)
        || mcode == UserMCode_Generic6
        || mcode == UserMCode_Generic8
#endif
#if defined(SOLDER_PASTE_CLAMP_PORT)
        || mcode == UserMCode_Generic7
        || mcode == UserMCode_Generic9
#endif
         ? UserMCode_Normal
         : (user_mcode.check ? user_mcode.check(mcode) : UserMCode_Unsupported);
}

static status_code_t pump_mcode_validate (parser_block_t *gc_block)
{
    status_code_t state = Status_OK;

    switch(gc_block->user_mcode) {

        case UserMCode_Generic1: // M101 - pressure mode, auto-start pump
        case UserMCode_Generic2: // M102 - vacuum mode, auto-start pump
        case UserMCode_Generic3: // M103 - stop pump and de-energize both valves
            gc_block->user_mcode_sync = On;
            break;

#if defined(AIRSLIDE1_PORT)
        case UserMCode_Generic4: // M104 - AIRSLIDE1 on, pressure mode required
            if(pump_state != PumpState_Pressure)
                state = Status_InvalidStatement;
            else
                gc_block->user_mcode_sync = On;
            break;

        case UserMCode_Generic5: // M105 - AIRSLIDE1 off
            gc_block->user_mcode_sync = On;
            break;
#endif

#if defined(SILVER_PASTE_CLAMP_PORT)
        case UserMCode_Generic6: // M108 - silver paste clamp pulse
            if(gc_block->words.p) {
                if(gc_block->values.p <= 0.0f)
                    state = Status_GcodeValueOutOfRange;
                else
                    gc_block->words.p = Off;
            } else
                gc_block->values.p = (float)SILVER_PASTE_CLAMP_PULSE_MS;

            if(state == Status_OK)
                gc_block->user_mcode_sync = On;
            break;

        case UserMCode_Generic8: // M110 - silver paste clamp off
            gc_block->user_mcode_sync = On;
            break;
#endif

#if defined(SOLDER_PASTE_CLAMP_PORT)
        case UserMCode_Generic7: // M109 - solder paste clamp pulse
            if(gc_block->words.p) {
                if(gc_block->values.p <= 0.0f)
                    state = Status_GcodeValueOutOfRange;
                else
                    gc_block->words.p = Off;
            } else
                gc_block->values.p = (float)SOLDER_PASTE_CLAMP_PULSE_MS;

            if(state == Status_OK)
                gc_block->user_mcode_sync = On;
            break;

        case UserMCode_Generic9: // M111 - solder paste clamp off
            gc_block->user_mcode_sync = On;
            break;
#endif

        default:
            state = Status_Unhandled;
            break;
    }

    return state == Status_Unhandled && user_mcode.validate ? user_mcode.validate(gc_block) : state;
}

static void pump_mcode_execute (sys_state_t state, parser_block_t *gc_block)
{
    bool handled = true;

    if(state != STATE_CHECK_MODE) {
        switch(gc_block->user_mcode) {

            case UserMCode_Generic1:
                pump_apply_state(PumpState_Pressure);
                report_message("Pump mode: pressure", Message_Info);
                break;

            case UserMCode_Generic2:
                pump_apply_state(PumpState_Vacuum);
                report_message("Pump mode: vacuum", Message_Info);
                break;

            case UserMCode_Generic3:
                pump_apply_state(PumpState_Off);
                report_message("Pump mode: off", Message_Info);
                break;

#if defined(AIRSLIDE1_PORT)
            case UserMCode_Generic4:
                if(pump_state == PumpState_Pressure) {
                    pump_set_airslide1(true);
                    report_message("Airslide1: on", Message_Info);
                }
                break;

            case UserMCode_Generic5:
                pump_set_airslide1(false);
                report_message("Airslide1: off", Message_Info);
                break;
#endif

#if defined(SILVER_PASTE_CLAMP_PORT)
            case UserMCode_Generic6:
                if(pump_trigger_pulse(silver_paste_clamp_off, SILVER_PASTE_CLAMP_PORT, SILVER_PASTE_CLAMP_PIN, SILVER_PASTE_CLAMP_ACTIVE_LEVEL, (uint32_t)gc_block->values.p))
                    report_message("Silver paste pulse", Message_Info);
                break;

            case UserMCode_Generic8:
                pump_set_silver_paste_clamp(false);
                report_message("Silver paste: off", Message_Info);
                break;
#endif

#if defined(SOLDER_PASTE_CLAMP_PORT)
            case UserMCode_Generic7:
                if(pump_trigger_pulse(solder_paste_clamp_off, SOLDER_PASTE_CLAMP_PORT, SOLDER_PASTE_CLAMP_PIN, SOLDER_PASTE_CLAMP_ACTIVE_LEVEL, (uint32_t)gc_block->values.p))
                    report_message("Solder paste pulse", Message_Info);
                break;

            case UserMCode_Generic9:
                pump_set_solder_paste_clamp(false);
                report_message("Solder paste: off", Message_Info);
                break;
#endif

            default:
                handled = false;
                break;
        }
    }

    if(!handled && user_mcode.execute)
        user_mcode.execute(state, gc_block);
}

static bool pump_driver_setup (settings_t *settings)
{
    bool ok = driver_setup ? driver_setup(settings) : false;

    if(ok) {
        pump_apply_state(PumpState_Off);
#if defined(AIRSLIDE1_PORT)
        pump_set_airslide1(false);
#endif
#if defined(SILVER_PASTE_CLAMP_PORT)
        task_delete(silver_paste_clamp_off, NULL);
        pump_set_silver_paste_clamp(false);
#endif
#if defined(SOLDER_PASTE_CLAMP_PORT)
        task_delete(solder_paste_clamp_off, NULL);
        pump_set_solder_paste_clamp(false);
#endif
    }

    return ok;
}

static void pump_driver_reset (void)
{
    driver_reset();
    pump_apply_state(PumpState_Off);
#if defined(AIRSLIDE1_PORT)
    pump_set_airslide1(false);
#endif
#if defined(SILVER_PASTE_CLAMP_PORT)
    task_delete(silver_paste_clamp_off, NULL);
    pump_set_silver_paste_clamp(false);
#endif
#if defined(SOLDER_PASTE_CLAMP_PORT)
    task_delete(solder_paste_clamp_off, NULL);
    pump_set_solder_paste_clamp(false);
#endif
}

static void pump_report_options (bool newopt)
{
    on_report_options(newopt);

    if(!newopt)
        report_plugin(
#if defined(AIRSLIDE1_PORT)
            "Pump control (M101/M102/M103, M104/M105, M108 Pnn/M110, M109 Pnn/M111)",
#else
            "Pump control (M101/M102/M103, M108 Pnn/M110, M109 Pnn/M111)",
#endif
            "0.07"
        );
}

void my_plugin_init (void)
{
    memcpy(&user_mcode, &grbl.user_mcode, sizeof(user_mcode_ptrs_t));

    grbl.user_mcode.check = pump_mcode_check;
    grbl.user_mcode.validate = pump_mcode_validate;
    grbl.user_mcode.execute = pump_mcode_execute;

    driver_setup = hal.driver_setup;
    hal.driver_setup = pump_driver_setup;

    driver_reset = hal.driver_reset;
    hal.driver_reset = pump_driver_reset;

    on_report_options = grbl.on_report_options;
    grbl.on_report_options = pump_report_options;
}

#else

void my_plugin_init (void)
{
    // No-op when the board does not define pump valve outputs.
}

#endif
