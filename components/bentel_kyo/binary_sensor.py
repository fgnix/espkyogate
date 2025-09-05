import esphome.config_validation as cv
from esphome.components import binary_sensor
import esphome.codegen as cg

from esphome.const import (
	DEVICE_CLASS_BATTERY,
	DEVICE_CLASS_MOTION,
	DEVICE_CLASS_POWER,
	DEVICE_CLASS_PROBLEM,
	DEVICE_CLASS_RUNNING,
	DEVICE_CLASS_TAMPER,
	ENTITY_CATEGORY_DIAGNOSTIC,
)

from . import (
	MAX_ZONES,
	MAX_PARTITIONS,
	CONF_PARTITION_ALARM_x,
	CONF_BENTEL_KYO_ID,
	CONF_OPERATIONAL,
	CONF_TAMPER_ZONE,
	CONF_TAMPER_FAKE_KEY,
	CONF_TAMPER_BPI,
	CONF_TAMPER_SYSTEM,
	CONF_TAMPER_JAM,
	CONF_TAMPER_WIRELESS,
	CONF_WARN_AC_POWER_LOSS,
	CONF_WARN_MISSING_BPI,
	CONF_WARN_FUSE,
	CONF_WARN_LOW_BATTERY,
	CONF_WARN_TELEPHONE_LINE,
	CONF_WARN_DEFAULT_CODES,
	CONF_WARN_WIRELESS,
	CONF_ZONE_x,
	CONF_ZONE_TAMPER_x,
	CONF_ZONE_BYPASSED_x,
	CONF_ZONE_ALARM_MEMORY_x,
	CONF_ZONE_TAMPER_MEMORY_x,
	BentelKyo,
)

DEPENDENCIES = ["bentel_kyo"]

# Expand to all 32 zones
ZONES_CONFIG_SCHEMA = (
	cv.Schema({
		cv.Optional(CONF_ZONE_x + str(i)): binary_sensor.binary_sensor_schema(
			device_class=DEVICE_CLASS_MOTION,
		)
		for i in range(1, MAX_ZONES+1)
	})
	.extend({
		cv.Optional(CONF_ZONE_TAMPER_x + str(i)): binary_sensor.binary_sensor_schema(
			device_class=DEVICE_CLASS_MOTION,
		)
		for i in range(1, MAX_ZONES+1)
	})
	.extend({
		cv.Optional(CONF_ZONE_BYPASSED_x + str(i)): binary_sensor.binary_sensor_schema(
			device_class=DEVICE_CLASS_MOTION,
		)
		for i in range(1, MAX_ZONES+1)
	})
	.extend({
		cv.Optional(CONF_ZONE_ALARM_MEMORY_x + str(i)): binary_sensor.binary_sensor_schema(
			device_class=DEVICE_CLASS_MOTION,
		)
		for i in range(1, MAX_ZONES+1)
	})
	.extend({
		cv.Optional(CONF_ZONE_TAMPER_MEMORY_x + str(i)): binary_sensor.binary_sensor_schema(
			device_class=DEVICE_CLASS_MOTION,
		)
		for i in range(1, MAX_ZONES+1)
	})
)

# Espand to all 8 partitions
PARTITIONS_CONFIG_SCHEMA = cv.Schema(
	{
		cv.Optional(CONF_PARTITION_ALARM_x + str(i)): binary_sensor.binary_sensor_schema()
		for i in range(1, MAX_PARTITIONS+1)
	}
)


CONFIG_SCHEMA = (
	cv.Schema(
		{
			cv.GenerateID(CONF_BENTEL_KYO_ID): cv.use_id(BentelKyo),
			cv.Optional(CONF_OPERATIONAL): binary_sensor.binary_sensor_schema(
				device_class=DEVICE_CLASS_RUNNING,
				entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
			),

			# Warnings
			cv.Optional(CONF_WARN_AC_POWER_LOSS): binary_sensor.binary_sensor_schema(
				device_class=DEVICE_CLASS_POWER,
				entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
			),
			cv.Optional(CONF_WARN_MISSING_BPI): binary_sensor.binary_sensor_schema(
				entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
			),
			cv.Optional(CONF_WARN_FUSE): binary_sensor.binary_sensor_schema(
				device_class=DEVICE_CLASS_PROBLEM,
				entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
			),
			cv.Optional(CONF_WARN_LOW_BATTERY): binary_sensor.binary_sensor_schema(
				device_class=DEVICE_CLASS_BATTERY,
				entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
			),
			cv.Optional(CONF_WARN_TELEPHONE_LINE): binary_sensor.binary_sensor_schema(
				entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
			),
			cv.Optional(CONF_WARN_DEFAULT_CODES): binary_sensor.binary_sensor_schema(
				entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
			),
			cv.Optional(CONF_WARN_WIRELESS): binary_sensor.binary_sensor_schema(
				entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
			),

			# Tamper
			cv.Optional(CONF_TAMPER_ZONE): binary_sensor.binary_sensor_schema(
				device_class=DEVICE_CLASS_TAMPER,
			),
			cv.Optional(CONF_TAMPER_FAKE_KEY): binary_sensor.binary_sensor_schema(
				device_class=DEVICE_CLASS_TAMPER,
			),
			cv.Optional(CONF_TAMPER_BPI): binary_sensor.binary_sensor_schema(
				device_class=DEVICE_CLASS_TAMPER,
			),
			cv.Optional(CONF_TAMPER_SYSTEM): binary_sensor.binary_sensor_schema(
				device_class=DEVICE_CLASS_TAMPER,
			),
			cv.Optional(CONF_TAMPER_JAM): binary_sensor.binary_sensor_schema(
				device_class=DEVICE_CLASS_TAMPER,
			),
			cv.Optional(CONF_TAMPER_WIRELESS): binary_sensor.binary_sensor_schema(
				device_class=DEVICE_CLASS_TAMPER,
			),
		}
	)
	.extend(ZONES_CONFIG_SCHEMA)
	.extend(PARTITIONS_CONFIG_SCHEMA)
)


async def to_code(config):
	parent = await cg.get_variable(config[CONF_BENTEL_KYO_ID])

	if operational := config.get(CONF_OPERATIONAL):
		sens = await binary_sensor.new_binary_sensor(operational)
		cg.add(parent.set_operational_binary_sensor(sens))

	# Zones
	for i in range(1, MAX_ZONES+1):
		if zone := config.get(CONF_ZONE_x + str(i)):
			sens = await binary_sensor.new_binary_sensor(zone)
			cg.add(parent.set_zone_sensor(sens, i))

	for i in range(1, MAX_ZONES+1):
		if zone := config.get(CONF_ZONE_TAMPER_x + str(i)):
			sens = await binary_sensor.new_binary_sensor(zone)
			cg.add(parent.set_zone_tamper_sensor(sens, i))

	for i in range(1, MAX_ZONES+1):
		if zone := config.get(CONF_ZONE_BYPASSED_x + str(i)):
			sens = await binary_sensor.new_binary_sensor(zone)
			cg.add(parent.set_zone_bypassed_sensor(sens, i))

	for i in range(1, MAX_ZONES+1):
		if zone := config.get(CONF_ZONE_ALARM_MEMORY_x + str(i)):
			sens = await binary_sensor.new_binary_sensor(zone)
			cg.add(parent.set_zone_alarm_memory_sensor(sens, i))

	for i in range(1, MAX_ZONES+1):
		if zone := config.get(CONF_ZONE_TAMPER_MEMORY_x + str(i)):
			sens = await binary_sensor.new_binary_sensor(zone)
			cg.add(parent.set_zone_tamper_memory_sensor(sens, i))

	# Partitions
	for i in range(1, MAX_PARTITIONS+1):
		if partition := config.get(CONF_PARTITION_ALARM_x + str(i)):
			sens = await binary_sensor.new_binary_sensor(partition)
			cg.add(parent.set_partition_alarm_sensor(sens, i))

	# Warnings
	if warn := config.get(CONF_WARN_AC_POWER_LOSS):
		sens = await binary_sensor.new_binary_sensor(warn)
		cg.add(parent.set_warn_ac_power_loss_binary_sensor(sens))
	if warn := config.get(CONF_WARN_MISSING_BPI):
		sens = await binary_sensor.new_binary_sensor(warn)
		cg.add(parent.set_warn_missing_bpi_binary_sensor(sens))
	if warn := config.get(CONF_WARN_FUSE):
		sens = await binary_sensor.new_binary_sensor(warn)
		cg.add(parent.set_warn_fuse_binary_sensor(sens))
	if warn := config.get(CONF_WARN_LOW_BATTERY):
		sens = await binary_sensor.new_binary_sensor(warn)
		cg.add(parent.set_warn_low_battery_binary_sensor(sens))
	if warn := config.get(CONF_WARN_TELEPHONE_LINE):
		sens = await binary_sensor.new_binary_sensor(warn)
		cg.add(parent.set_warn_faulty_telephone_line_binary_sensor(sens))
	if warn := config.get(CONF_WARN_DEFAULT_CODES):
		sens = await binary_sensor.new_binary_sensor(warn)
		cg.add(parent.set_warn_default_codes_binary_sensor(sens))
	if warn := config.get(CONF_WARN_WIRELESS):
		sens = await binary_sensor.new_binary_sensor(warn)
		cg.add(parent.set_warn_wireless_binary_sensor(sens))

	# Tamper
	if sabot := config.get(CONF_TAMPER_ZONE):
		sens = await binary_sensor.new_binary_sensor(sabot)
		cg.add(parent.set_tamper_zone_binary_sensor(sens))
	if sabot := config.get(CONF_TAMPER_FAKE_KEY):
		sens = await binary_sensor.new_binary_sensor(sabot)
		cg.add(parent.set_tamper_fake_key_binary_sensor(sens))
	if sabot := config.get(CONF_TAMPER_BPI):
		sens = await binary_sensor.new_binary_sensor(sabot)
		cg.add(parent.set_tamper_bpi_binary_sensor(sens))
	if sabot := config.get(CONF_TAMPER_SYSTEM):
		sens = await binary_sensor.new_binary_sensor(sabot)
		cg.add(parent.set_tamper_system_binary_sensor(sens))
	if sabot := config.get(CONF_TAMPER_JAM):
		sens = await binary_sensor.new_binary_sensor(sabot)
		cg.add(parent.set_tamper_jam_binary_sensor(sens))
	if sabot := config.get(CONF_TAMPER_WIRELESS):
		sens = await binary_sensor.new_binary_sensor(sabot)
		cg.add(parent.set_tamper_wireless_binary_sensor(sens))
