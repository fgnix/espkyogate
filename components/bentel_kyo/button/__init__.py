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
	CONF_PARTITIONS_ARM_COMMIT,
	CONF_ALL_ALARMS_RESET,
	CONF_CLOCK_UPDATE,
	bentel_kyo_ns,
	BentelKyo,
)

DEPENDENCIES = ["bentel_kyo"]

PartitionsArmCommitButton = bentel_kyo_ns.class_("PartitionsArmCommitButton", button.Button)
ClockUpdateButton = bentel_kyo_ns.class_("ClockUpdateButton", button.Button)
AllAlarmsResetButton = bentel_kyo_ns.class_("AllAlarmsResetButton", button.Button)

CONFIG_SCHEMA = cv.Schema(
	{
		cv.GenerateID(CONF_BENTEL_KYO_ID): cv.use_id(BentelKyo),
		cv.Optional(CONF_PARTITIONS_ARM_COMMIT): button.button_schema(
			PartitionsArmCommitButton,
			icon="mdi:shield-lock",
		),
		cv.Optional(CONF_CLOCK_UPDATE): button.button_schema(
			ClockUpdateButton,
			icon="mdi:clock",
			entity_category=ENTITY_CATEGORY_CONFIG,
		),
		cv.Optional(CONF_ALL_ALARMS_RESET): button.button_schema(
			AllAlarmsResetButton,
			icon="mdi:alarm-light-off",
		),
	}
)

async def to_code(config):
	parent = await cg.get_variable(config[CONF_BENTEL_KYO_ID])

	if paritions_arm_commit := config.get(CONF_PARTITIONS_ARM_COMMIT):
		btn = await button.new_button(paritions_arm_commit)
		await cg.register_parented(btn, parent)
		cg.add(parent.set_paritions_arm_commit_button(btn))

	if clock_update := config.get(CONF_CLOCK_UPDATE):
		btn = await button.new_button(clock_update)
		await cg.register_parented(btn, parent)
		cg.add(parent.set_clock_update_button(btn))

	if all_alarms_reset := config.get(CONF_ALL_ALARMS_RESET):
		btn = await button.new_button(all_alarms_reset)
		await cg.register_parented(btn, parent)
		cg.add(parent.set_all_alarms_reset_button(btn))
