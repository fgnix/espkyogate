import esphome.config_validation as cv
from esphome.components import binary_sensor
import esphome.codegen as cg

from esphome.const import (
	DEVICE_CLASS_MOTION,
	DEVICE_CLASS_RUNNING,
	ENTITY_CATEGORY_DIAGNOSTIC,
)

from . import (
	MAX_ZONES,
	MAX_PARTITIONS,
	CONF_BENTEL_KYO_ID,
	CONF_OPERATIONAL,
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

CONFIG_SCHEMA = (
	cv.Schema(
		{
			cv.GenerateID(CONF_BENTEL_KYO_ID): cv.use_id(BentelKyo),
			cv.Optional(CONF_OPERATIONAL): binary_sensor.binary_sensor_schema(
				device_class=DEVICE_CLASS_RUNNING,
				entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
			),
		}
	)
	.extend(ZONES_CONFIG_SCHEMA)
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
