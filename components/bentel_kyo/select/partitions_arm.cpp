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

#include "partitions_arm.h"

#ifdef USE_SELECT

namespace esphome {
namespace bentel_kyo {

PartitionArmSelect::PartitionArmSelect(const uint8_t partition_id): partition_id_(partition_id) {}

void PartitionArmSelect::control(const std::string &value) {
	if (value == partitions_arm_mode::ARM_AWAY) {
		this->parent_->partition_arm_edit(this->partition_id_, PartitionsArmMode::AWAY);
	} else if (value == partitions_arm_mode::ARMED_STAY) {
		this->parent_->partition_arm_edit(this->partition_id_, PartitionsArmMode::STAY);
	} else if (value == partitions_arm_mode::ARMED_STAY_0_DELAY) {
		this->parent_->partition_arm_edit(this->partition_id_, PartitionsArmMode::STAY_0_DELAY);
	} else if (value == partitions_arm_mode::DISARM) {
		this->parent_->partition_arm_edit(this->partition_id_, PartitionsArmMode::DISARM);
	} else {
		this->parent_->partition_arm_edit(this->partition_id_, PartitionsArmMode::NO_CHANGE);
		this->publish_state(partitions_arm_mode::NO_CHANGE);
		return;
	}
	this->publish_state(value);
}

}  // namespace bentel_kyo
}  // namespace esphome

#endif
