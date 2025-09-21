import esphome.config_validation as cv
from esphome.components import button
import esphome.codegen as cg

from esphome.const import (
	DEVICE_CLASS_TIMESTAMP,
	ENTITY_CATEGORY_CONFIG,
)

from .. import (
	MAX_ZONES,
	MAX_PARTITIONS,
	CONF_BENTEL_KYO_ID,
	CONF_CLOCK_UPDATE,
	bentel_kyo_ns,
	BentelKyo,
)

DEPENDENCIES = ["bentel_kyo"]

ClockUpdateButton = bentel_kyo_ns.class_("ClockUpdateButton", button.Button)

CONFIG_SCHEMA = cv.Schema(
	{
		cv.GenerateID(CONF_BENTEL_KYO_ID): cv.use_id(BentelKyo),
		cv.Optional(CONF_CLOCK_UPDATE): button.button_schema(
			ClockUpdateButton,
			icon="mdi:clock",
			entity_category=ENTITY_CATEGORY_CONFIG,
		),
	}
)

async def to_code(config):
	parent = await cg.get_variable(config[CONF_BENTEL_KYO_ID])

	if clock_update := config.get(CONF_CLOCK_UPDATE):
		btn = await button.new_button(clock_update)
		await cg.register_parented(btn, parent)
		cg.add(parent.set_clock_update_button(btn))
