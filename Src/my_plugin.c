#include "driver.h"

#include <string.h>

#include "grbl/core_handlers.h"
#include "grbl/report.h"

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

    if(ok)
        pump_apply_state(PumpState_Off);

    return ok;
}

static void pump_driver_reset (void)
{
    driver_reset();
    pump_apply_state(PumpState_Off);
}

static void pump_report_options (bool newopt)
{
    on_report_options(newopt);

    if(!newopt)
        report_plugin("Pump control (M101/M102/M103)", "0.03");
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
