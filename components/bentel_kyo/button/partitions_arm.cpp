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

#ifdef USE_BUTTON
#ifdef USE_SELECT

namespace esphome {
namespace bentel_kyo {

void PartitionsArmCommitButton::press_action() { this->parent_->schedule_partitions_arm_commit(); }

}  // namespace bentel_kyo
}  // namespace esphome

#endif
#endif
