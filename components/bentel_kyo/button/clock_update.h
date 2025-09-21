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

#ifdef USE_TIME
#ifdef USE_BUTTON

namespace esphome {
namespace bentel_kyo {

class ClockUpdateButton : public button::Button, public Parented<BentelKyo> {
	public:
		ClockUpdateButton() = default;

	protected:
		void press_action() override;
};

}  // namespace bentel_kyo
}  // namespace esphome

#endif
#endif
