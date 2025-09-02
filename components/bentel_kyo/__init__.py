import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import uart

from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
MULTI_CONF = True

MAX_PARTITIONS = 8 # a.k.a Areas
MAX_ZONES = 32

CONF_BENTEL_KYO_ID = "bentel_kyo_id"
CONF_MODEL = "model"
CONF_OPERATIONAL = "operational"
CONF_PARTITION_ALARM_x = "partition_alarm_"
CONF_PARTITION_ARMED_STATUS_x = "partition_armed_status_"
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
BentelKyo = bentel_kyo_ns.class_("BentelKyo", cg.PollingComponent, uart.UARTDevice)

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


CONFIG_SCHEMA = (
	cv.Schema(
		{
			cv.GenerateID(): cv.declare_id(BentelKyo),
			cv.Required(CONF_MODEL): cv.enum(text2AlarmModel),
		}
	)
	.extend(cv.polling_component_schema("10s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
	zones_num = Model2ZonesNum[config[CONF_MODEL]]
	partitions_num = Model2PartitionsNum[config[CONF_MODEL]]

	var = cg.new_Pvariable(config[CONF_ID], config[CONF_MODEL], zones_num, partitions_num)
	await cg.register_component(var, config)
	await uart.register_uart_device(var, config)
