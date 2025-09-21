from esphome import automation
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import uart
from esphome.components import time

from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL

DEPENDENCIES = ["uart"]
MULTI_CONF = True

MAX_PARTITIONS = 8 # a.k.a Areas
MAX_ZONES = 32

CONF_ALL_ALARMS_RESET = "all_alarms_reset"
CONF_BENTEL_KYO_ID = "bentel_kyo_id"
CONF_DRY_RUN = "dry_run"
CONF_CLOCK_UPDATE = "clock_update"
CONF_GLOBAL_ALARM = "global_alarm"
CONF_LANG = "lang"
CONF_MODEL = "model"
CONF_OPERATIONAL = "operational"
CONF_PARTITION_x = "partition_"
CONF_PARTITION_ALARM_x = "partition_alarm_"
CONF_PARTITION_ARM_x = "partition_arm_"
CONF_PARTITIONS_ARM_COMMIT = "partitions_arm_commit"
CONF_PARTITION_ARMED_STATUS_x = "partition_armed_status_"
CONF_PARTITIONS_UPDATE_SKIP = "partitions_update_skip"
CONF_REAL_TIME_CLOCK_ID = "real_time_clock_id"
CONF_TAMPER_ZONE = "tamper_zone"
CONF_TAMPER_FAKE_KEY = "tamper_fake_key"
CONF_TAMPER_BPI = "tamper_bpi"
CONF_TAMPER_SYSTEM = "tamper_system"
CONF_TAMPER_JAM = "tamper_jam"
CONF_TAMPER_WIRELESS = "tamper_wireless"
CONF_WARN_AC_POWER_LOSS = "warn_ac_power_loss"
CONF_WARN_MISSING_BPI = "warn_missing_bpi"
CONF_WARN_FUSE = "warn_fuse"
CONF_WARN_LOW_BATTERY = "warn_low_battery"
CONF_WARN_TELEPHONE_LINE = "warn_telephone_line"
CONF_WARN_DEFAULT_CODES = "warn_defualt_codes"
CONF_WARN_WIRELESS = "warn_wireless"
CONF_ZONE_x = "zone_"
CONF_ZONE_ALARM_MEMORY_x = "zone_alarm_memory_"
CONF_ZONE_BYPASSED_x = "zone_bypassed_"
CONF_ZONE_TAMPER_x = "zone_tamper_"
CONF_ZONE_TAMPER_MEMORY_x = "zone_tamper_memory_"

bentel_kyo_ns = cg.esphome_ns.namespace("bentel_kyo")
AlarmModel = bentel_kyo_ns.enum("AlarmModel", is_class=True)
PartitionsArmMode = bentel_kyo_ns.enum("PartitionsArmMode", is_class=True)
BentelKyo = bentel_kyo_ns.class_("BentelKyo", cg.PollingComponent, uart.UARTDevice)
ClockUpdateAction = bentel_kyo_ns.class_("ClockUpdateAction", automation.Action)
AllAlarmsResetAction = bentel_kyo_ns.class_("AllAlarmsResetAction", automation.Action)
PartitionsArmedEditAction = bentel_kyo_ns.class_("PartitionsArmedEditAction", automation.Action)

text2AlarmModel = {
	"Kyo4": AlarmModel.KYO_4,
	"Kyo8": AlarmModel.KYO_8,
	"Kyo8G": AlarmModel.KYO_8G,
	"Kyo8W": AlarmModel.KYO_8W,
	"Kyo8GW": AlarmModel.KYO_8GW,
	"Kyo32": AlarmModel.KYO_32,
	"Kyo32G": AlarmModel.KYO_32G,
}

Model2ZonesNum = {
	"Kyo4":   4,
	"Kyo8":   8,
	"Kyo8G":  8,
	"Kyo8W":  8,
	"Kyo8GW": 8,
	"Kyo32":  32,
	"Kyo32G": 32,
}

Model2PartitionsNum = {
	"Kyo4":   4,
	"Kyo8":   4,
	"Kyo8G":  4,
	"Kyo8W":  4,
	"Kyo8GW": 4,
	"Kyo32":  8,
	"Kyo32G": 8,
}

SupportedLangs = (
	"en",
	"it",
)

text2PartitionsArmMode = {
	"no_change": PartitionsArmMode.NO_CHANGE,
	"total": PartitionsArmMode.AWAY,
	"away": PartitionsArmMode.AWAY,
	"partial": PartitionsArmMode.STAY,
	"stay": PartitionsArmMode.STAY,
	"partial_0_delay": PartitionsArmMode.STAY_0_DELAY,
	"stay_0_delay": PartitionsArmMode.STAY_0_DELAY,
	"disarm": PartitionsArmMode.DISARM,
};

CONF_SELECTS_PARTITION_ARMED_MODES = {
	"it": [
		"No change",
		"Inserisci",
		"Parziale",
		"Parziale 0 ritardo",
		"Disinserici",
	],
	"en": [
		"No change",
		"Away",
		"Stay",
		"Stay 0 delay",
		"Disarm",
	],
}

CONFIG_SCHEMA = (
	cv.Schema(
		{
			cv.GenerateID(): cv.declare_id(BentelKyo),
			cv.Required(CONF_MODEL): cv.enum(text2AlarmModel),
			cv.Optional(CONF_LANG, default="en"): cv.one_of(*SupportedLangs, lower=True),
			cv.Optional(CONF_DRY_RUN, default="no"): cv.boolean,
			cv.Optional(CONF_PARTITIONS_UPDATE_SKIP): cv.int_range(min=0, max=250),
			cv.Optional(CONF_CLOCK_UPDATE): cv.Schema(
				{
					cv.Required(CONF_REAL_TIME_CLOCK_ID): cv.use_id(time.RealTimeClock),
					cv.Optional(CONF_UPDATE_INTERVAL, default="1days"): cv.update_interval,
				}
			),
		}
	)
	.extend(cv.polling_component_schema("10s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

# Actions schema
SIMPLE_ACTION_SCHEMA = automation.maybe_simple_id(
	{
		cv.Required(CONF_BENTEL_KYO_ID): cv.use_id(BentelKyo),
	}
)
PARTITIONS_ARM_ACTION_SCHEMA = (
	cv.Schema(
		{
			cv.Required(CONF_BENTEL_KYO_ID): cv.use_id(BentelKyo),
		}
	)
	.extend(
		{
			cv.Optional(CONF_PARTITION_x + str(i)): cv.enum(text2PartitionsArmMode)
			for i in range(1, MAX_PARTITIONS+1)
		}
	)
)

async def to_code(config):
	zones_num = Model2ZonesNum[config[CONF_MODEL]]
	partitions_num = Model2PartitionsNum[config[CONF_MODEL]]

	if config.get(CONF_DRY_RUN):
		cg.add_define("DRY_RUN")

	match config.get(CONF_LANG):
		case "it":
			cg.add_define("LANG_IT")

	var = cg.new_Pvariable(config[CONF_ID], config[CONF_MODEL], zones_num, partitions_num)
	await cg.register_component(var, config)
	await uart.register_uart_device(var, config)

	if partitions_update_skip := config.get(CONF_PARTITIONS_UPDATE_SKIP):
		cg.add(var.set_partitions_update_skip(partitions_update_skip))

	# Clock update section
	if clock_update_config := config.get(CONF_CLOCK_UPDATE):
		rtc = await cg.get_variable(clock_update_config[CONF_REAL_TIME_CLOCK_ID])
		cg.add(var.set_real_time_clock(rtc))
		cg.add(var.set_clock_update_interval(clock_update_config[CONF_UPDATE_INTERVAL]))


@automation.register_action(
	"bentel_kyo.clock_update",
	ClockUpdateAction,
	SIMPLE_ACTION_SCHEMA,
)
@automation.register_action(
	"bentel_kyo.all_alarms_reset",
	AllAlarmsResetAction,
	SIMPLE_ACTION_SCHEMA,
)
async def bentel_kyo_action_to_code(config, action_id, template_arg, args):
	paren = await cg.get_variable(config[CONF_BENTEL_KYO_ID])
	return cg.new_Pvariable(action_id, template_arg, paren)


@automation.register_action("bentel_kyo.partitions_aremd_edit", PartitionsArmedEditAction, PARTITIONS_ARM_ACTION_SCHEMA)
async def bentel_kyo_change_armed_status_action_to_code(config, action_id, template_arg, args):
	paren = await cg.get_variable(config[CONF_BENTEL_KYO_ID])
	var = cg.new_Pvariable(action_id, template_arg, paren)

	for i in range(1, MAX_PARTITIONS+1):
		if (
			(partition_mode := config.get(CONF_PARTITION_x + str(i)))
			and text2PartitionsArmMode[partition_mode] != PartitionsArmMode.NO_CHANGE
		):
			cg.add(var.set_partition_armed_mode(i, partition_mode))

	return var
