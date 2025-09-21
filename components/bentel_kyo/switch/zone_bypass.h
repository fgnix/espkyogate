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

#pragma once
#include "../bentel_kyo.h"

#ifdef USE_SWITCH

namespace esphome {
namespace bentel_kyo {

class ZoneBypassSwitch : public switch_::Switch, public Parented<BentelKyo> {
	public:
		ZoneBypassSwitch(const uint8_t zone_id);
		switch_::SwitchRestoreMode restore_mode{switch_::SWITCH_RESTORE_DISABLED};

	protected:
		const uint8_t zone_id_;

		void write_state(bool state) override;
};

}  // namespace bentel_kyo
}  // namespace esphome

#endif
