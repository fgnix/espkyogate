import esphome.config_validation as cv
from esphome.components import switch
import esphome.codegen as cg

from esphome.const import (
	DEVICE_CLASS_TIMESTAMP,
	ENTITY_CATEGORY_CONFIG,
)

from .. import (
	MAX_ZONES,
	CONF_BENTEL_KYO_ID,
	CONF_ZONE_BYPASS_x,
	bentel_kyo_ns,
	BentelKyo,
)

DEPENDENCIES = ["bentel_kyo"]

ZoneBypassSwitch = bentel_kyo_ns.class_("ZoneBypassSwitch", switch.Switch)

CONFIG_SCHEMA = (
	cv.Schema(
		{
			cv.GenerateID(CONF_BENTEL_KYO_ID): cv.use_id(BentelKyo),
		}
	)
	.extend(
		{
			cv.Optional(CONF_ZONE_BYPASS_x + str(i)): switch.switch_schema(
				ZoneBypassSwitch,
				icon="mdi:home-off",
				entity_category=ENTITY_CATEGORY_CONFIG,
			)
			for i in range(1, MAX_ZONES+1)
		}
	)
)

async def to_code(config):
	parent = await cg.get_variable(config[CONF_BENTEL_KYO_ID])

	for i in range(1, MAX_ZONES+1):
		if zone := config.get(CONF_ZONE_BYPASS_x + str(i)):
			swt = await switch.new_switch(zone, i)
			cg.add(swt.set_parent(parent))
			cg.add(parent.set_zone_bypass_switch(swt, i))
