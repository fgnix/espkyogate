import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import uart

from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
MULTI_CONF = True

CONF_BENTEL_KYO_ID = "bentel_kyo_id"

bentel_kyo_ns = cg.esphome_ns.namespace("bentel_kyo")
BentelKyo = bentel_kyo_ns.class_("BentelKyo", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = (
	cv.Schema(
		{
			cv.GenerateID(): cv.declare_id(BentelKyo),
		}
	)
	.extend(cv.polling_component_schema("10s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
	var = cg.new_Pvariable(config[CONF_ID])
	await cg.register_component(var, config)
	await uart.register_uart_device(var, config)
