import esphome.config_validation as cv
from esphome.components import text_sensor
import esphome.codegen as cg

from esphome.const import (
	DEVICE_CLASS_MOTION,
	DEVICE_CLASS_RUNNING,
	ENTITY_CATEGORY_DIAGNOSTIC,
)

from . import (
	MAX_ZONES,
	MAX_PARTITIONS,
	CONF_PARTITION_ARMED_STATUS_x,
	CONF_BENTEL_KYO_ID,
	BentelKyo,
)

DEPENDENCIES = ["bentel_kyo"]

# Espand to all 8 partitions
PARTITIONS_CONFIG_SCHEMA = cv.Schema(
	{
		cv.Optional(CONF_PARTITION_ARMED_STATUS_x + str(i)): text_sensor.text_sensor_schema()
		for i in range(1, MAX_PARTITIONS+1)
	}
)


CONFIG_SCHEMA = (
	cv.Schema(
		{
			cv.GenerateID(CONF_BENTEL_KYO_ID): cv.use_id(BentelKyo),
		}
	)
	.extend(PARTITIONS_CONFIG_SCHEMA)
)


async def to_code(config):
	parent = await cg.get_variable(config[CONF_BENTEL_KYO_ID])

	for i in range(1, MAX_PARTITIONS+1):
		if partition := config.get(CONF_PARTITION_ARMED_STATUS_x + str(i)):
			sens = await text_sensor.new_text_sensor(partition)
			cg.add(parent.set_partition_armed_text_sensor(sens, i))
