import esphome.config_validation as cv
from esphome.components import binary_sensor
import esphome.codegen as cg

from esphome.const import (
	DEVICE_CLASS_MOTION,
	DEVICE_CLASS_RUNNING,
	ENTITY_CATEGORY_DIAGNOSTIC,
)

from . import (
	CONF_BENTEL_KYO_ID,
	CONF_OPERATIONAL,
	BentelKyo,
)

DEPENDENCIES = ["bentel_kyo"]


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
)


async def to_code(config):
	parent = await cg.get_variable(config[CONF_BENTEL_KYO_ID])

	if operational := config.get(CONF_OPERATIONAL):
		sens = await binary_sensor.new_binary_sensor(operational)
		cg.add(parent.set_operational_binary_sensor(sens))
