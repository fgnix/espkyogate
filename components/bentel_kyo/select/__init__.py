import esphome.config_validation as cv
from esphome.components import select
import esphome.codegen as cg

from esphome.const import (
	DEVICE_CLASS_TIMESTAMP,
	ENTITY_CATEGORY_CONFIG,
)

from .. import (
	MAX_ZONES,
	MAX_PARTITIONS,
	CONF_PARTITION_ARM_x,
	CONF_BENTEL_KYO_ID,
	bentel_kyo_ns,
	BentelKyo,
	CONF_SELECTS_PARTITION_ARMED_MODES,
)

DEPENDENCIES = ["bentel_kyo"]

PartitionArmSelect = bentel_kyo_ns.class_("PartitionArmSelect", select.Select)

CONFIG_SCHEMA = (
	cv.Schema(
		{
			cv.GenerateID(CONF_BENTEL_KYO_ID): cv.use_id(BentelKyo),
		}
	)
	.extend(
		{
			cv.Optional(CONF_PARTITION_ARM_x + str(i)): select.select_schema(
				PartitionArmSelect,
				icon="mdi:home-off", # TODO: change
			)
			for i in range(1, MAX_PARTITIONS+1)
		}
	)
)

async def to_code(config):
	parent = await cg.get_variable(config[CONF_BENTEL_KYO_ID])

	for i in range(1, MAX_PARTITIONS+1):
		if partition := config.get(CONF_PARTITION_ARM_x + str(i)):
			slt = await select.new_select(partition, i, options=CONF_SELECTS_PARTITION_ARMED_MODES["it"])
			cg.add(slt.set_parent(parent))
			cg.add(parent.set_partition_arm_select(slt, i))
