/*
* bentel_kyo - ESPHome component for Bentel KYO alarms
* Copyright (C) 2025 Lorenzo De Luca (me@lorenzodeluca.dev)
* Copyright (C) 2025 fgnix
*
* This program is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
* PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
*
* GNU Affero General Public License v3.0
*/

#include "bentel_kyo.h"


namespace esphome {
namespace bentel_kyo {

BentelKyo::BentelKyo(AlarmModel model, uint8_t max_zones, uint8_t max_partitions)
	: model_(model), max_partitions_(max_partitions), max_zones_(max_zones)
{

}

void BentelKyo::setup(){

}

void BentelKyo::update(){

}

void BentelKyo::dump_config(){
	ESP_LOGCONFIG(TAG,
	              "Bentel Kyo:\n"
	              "  model: %i\n"
	              "  max_zones: %i\n"
	              "  max_partitions: %i\n",
	              this->model_,
	              this->max_zones_,
	              this->max_partitions_);
}

}  // namespace bentel_kyo
}  // namespace esphome
