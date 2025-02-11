#include "Devices.h"

//#define DEBUG printf
#define DEBUG(format, args...) ((void)0)

Device Devices::getByName(const char* search, SearchType searchType) {
	auto devices = getByName2(search, searchType);
	return !devices.empty() ? devices.front() : Device("", "");
}

std::vector<Device> Devices::getByName2(const char* search, SearchType searchType) {

	std::vector<Device> devices;
	char name[512];
	auto files = Files::get_files(PATH_DEV_INPUT);

	for (const auto& file : files) {

		const char* path = file.c_str();
		
		int fd = open(path, O_RDONLY);
		if (fd == -1) {
			DEBUG("%s - failed to open device: %s (%s)\n", __func__, path, strerror(errno));
			continue;
		}

		std::memset(name, 0, sizeof(name));
		int result = ioctl(fd, EVIOCGNAME(sizeof(name)), name);
		close(fd);
		if (result == -1) {
			DEBUG("%s - io error device: %s\n", __func__, path);
			continue;
		}
		DEBUG("%s - reading from: %s (%s)\n", __func__, path, name);

		if ((searchType == SearchType::CONTAINS) && (strstr(name, search) != 0)) {
			DEBUG("%s - found '%s' %s (%s)\n", __func__, search, path, name);
			devices.push_back(Device(name, path));
		} else if ((searchType == SearchType::EXACT) && (strcmp(name, search) == 0)) {
			DEBUG("%s - found '%s' %s (%s)\n", __func__, search, path, name);
			devices.push_back(Device(name, path));
		}
	}

	DEBUG("%s - search: %s - found %zu devices\n", __func__, search, devices.size());
	return devices;
}

std::vector<Device> Devices::getWithKeysRelsSupported(const std::vector<int>& keys, const std::vector<int>& rels) {

	std::vector<Device> devices;
	char name[512];
	auto files = Files::get_files(PATH_DEV_INPUT);

	for (const auto& file : files) {

		const char* path = file.c_str();

		int fd = open(path, O_RDONLY);
		if (fd == -1) {
			DEBUG("%s - failed to open device: %s (%s)\n", __func__, path, strerror(errno));
			continue;
		}

		std::memset(name, 0, sizeof(name));
		int result = ioctl(fd, EVIOCGNAME(sizeof(name)), name);
		if (result == -1) {
			DEBUG("%s - io error device: %s\n", __func__, path);
			close(fd);
			continue;
		}
		DEBUG("%s - reading from: %s (%s)\n", __func__, path, name);
		
		bool isKeysSupported = true;
		bool isRelsSupported = true;
		for (const auto& key : keys) {
			if (!isKeySupported(fd, key)) {
				isKeysSupported = false;
				break;
			}
		}
		for (const auto& rel : rels) {
			if (!isRelSupported(fd, rel)) {
				isRelsSupported = false;
				break;
			}
		}
		
		if (isKeysSupported && isRelsSupported) {
			DEBUG("%s - found %s (%s)\n", __func__, path, name);
			devices.push_back(Device(name, path));
		}
		close(fd);
	}

	DEBUG("%s - found %zu devices\n", __func__, devices.size());
	return devices;
}

bool Devices::showDevices(DetailType detailType) {
	auto files = Files::get_files(PATH_DEV_INPUT);
	for (const auto& file : files) {
		Devices::showDevice(file.c_str(), detailType);
	}
	return true;
}

bool Devices::showDevice(const char* device, DetailType detailType) {
	
	int fd = open(device, O_RDONLY);
	if (fd == -1) {
		DEBUG("%s - failed to open device: %s (%s)\n", __func__, device, strerror(errno));
		return -1;
	}

	//////////////////////////////////////////////////
	// get device name
	// (EVIOCGNAME)
	//////////////////////////////////////////////////
	char name[512];
	std::memset(name, 0, sizeof(name));
	ioctl(fd, EVIOCGNAME(sizeof(name)), name);
	DEBUG ("%s reading from: %s (%s)\n", __func__, device, name);
	//////////////////////////////////////////////////

	if (detailType == DetailType::NAME) { return true; }

	//////////////////////////////////////////////////
	// get driver version
	// (EVIOCGVERSION)
	//////////////////////////////////////////////////
	int version;
	ioctl(fd, EVIOCGVERSION, &version);
	DEBUG("driver version - 0x%08x\n", version);
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get device id
	// (EVIOCGID)
	//////////////////////////////////////////////////
	struct input_id id;
	ioctl(fd, EVIOCGID, &id);
	DEBUG("device id - bustype:0x%04x product:0x%04x vendor:0x%04x version:0x%04x\n", id.bustype, id.product, id.vendor, id.version);
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get physical location
	// (EVIOCGPHYS)
	//////////////////////////////////////////////////
	char location[512];
	std::memset(location, 0, sizeof(location));
	ioctl(fd, EVIOCGPHYS(sizeof(location)), location);
	DEBUG ("location: %s\n", location);
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get unique identifier
	// (EVIOCGUNIQ)
	//////////////////////////////////////////////////
	char identifier[512];
	std::memset(identifier, 0, sizeof(identifier));
	ioctl(fd, EVIOCGUNIQ(sizeof(identifier)), identifier);
	DEBUG ("unique identifier: %s\n", identifier);
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get device properties
	// (EVIOCGUNIQ)
	// TODO: not working yet so check how this works!
	//////////////////////////////////////////////////
	char properties[512];
	std::memset(properties, 0, sizeof(properties));
	ioctl(fd, EVIOCGPROP(sizeof(properties)), properties);
	DEBUG ("device properties: %s\n", properties);
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get supported 'event types'
	// (EV_XXX, EV_MAX)
	//////////////////////////////////////////////////
	unsigned long etypes = 0;
	ioctl(fd, EVIOCGBIT(0, sizeof(etypes)), &etypes);
	DEBUG("supported events:\n%lx ", etypes);
	if (etypes & (1 << EV_SYN)) { DEBUG("EV_SYN "); }
	if (etypes & (1 << EV_KEY)) { DEBUG("EV_KEY "); }
	if (etypes & (1 << EV_REL)) { DEBUG("EV_REL "); }
	if (etypes & (1 << EV_ABS)) { DEBUG("EV_ABS "); }
	if (etypes & (1 << EV_MSC)) { DEBUG("EV_MSC "); }
	if (etypes & (1 << EV_SW))  { DEBUG("EV_SW "); }
	if (etypes & (1 << EV_LED)) { DEBUG("EV_LED "); }
	if (etypes & (1 << EV_SND)) { DEBUG("EV_SND "); }
	if (etypes & (1 << EV_REP)) { DEBUG("EV_REP "); }
	if (etypes & (1 << EV_FF))  { DEBUG("EV_FF "); }
	if (etypes & (1 << EV_PWR)) { DEBUG("EV_PWR "); }
	if (etypes & (1 << EV_FF_STATUS)) { DEBUG("EV_FF_STATUS "); }
	DEBUG("\n");

	//////////////////////////////////////////////////
	// get supported 'keys, buttons'
	// (EV_KEY = 0x01, KEY_MAX)
	// matches output from: cat /proc/bus/input/devices
	//////////////////////////////////////////////////
	if (etypes & (1 << EV_KEY)) {
		unsigned char key_caps[(KEY_MAX / 8) + 1];
		std::memset(key_caps, 0, sizeof(key_caps));
		DEBUG("supported keys:\n");

		if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(key_caps)), &key_caps) != -1) {
			[[maybe_unused]] unsigned long * ptr = (unsigned long *)&key_caps[(KEY_MAX / 8) + 1];
			for (size_t i = sizeof(key_caps); ; i -= sizeof(unsigned long)) {
				DEBUG("%lx ", *ptr);
				ptr--;
				if (i == 0) { break; }
			}
			DEBUG("\n");

			if (key_caps[BTN_MOUSE/8] & (1 << (BTN_MOUSE % 8))) { DEBUG("BTN_MOUSE "); }
			if (key_caps[BTN_LEFT/8] & (1 << (BTN_LEFT % 8))) { DEBUG("BTN_LEFT "); }
			if (key_caps[BTN_RIGHT/8] & (1 << (BTN_RIGHT % 8))) { DEBUG("BTN_RIGHT "); }
			if (key_caps[BTN_MIDDLE/8] & (1 << (BTN_MIDDLE % 8))) { DEBUG("BTN_MIDDLE "); }
			if (key_caps[BTN_WHEEL/8] & (1 << (BTN_WHEEL % 8))) { DEBUG("BTN_WHEEL "); }

			if (key_caps[BTN_MISC/8] & (1 << (BTN_MISC % 8))) { DEBUG("BTN_MISC "); }
			if (key_caps[BTN_0/8] & (1 << (BTN_0 % 8))) { DEBUG("BTN_0 "); }
			if (key_caps[BTN_1/8] & (1 << (BTN_1 % 8))) { DEBUG("BTN_1 "); }
			if (key_caps[BTN_2/8] & (1 << (BTN_2 % 8))) { DEBUG("BTN_2 "); }
			if (key_caps[BTN_3/8] & (1 << (BTN_3 % 8))) { DEBUG("BTN_3 "); }
			if (key_caps[BTN_4/8] & (1 << (BTN_4 % 8))) { DEBUG("BTN_4 "); }
			if (key_caps[BTN_5/8] & (1 << (BTN_5 % 8))) { DEBUG("BTN_5 "); }
			if (key_caps[BTN_6/8] & (1 << (BTN_6 % 8))) { DEBUG("BTN_6 "); }
			if (key_caps[BTN_7/8] & (1 << (BTN_7 % 8))) { DEBUG("BTN_7 "); }
			if (key_caps[BTN_8/8] & (1 << (BTN_8 % 8))) { DEBUG("BTN_8 "); }
			if (key_caps[BTN_9/8] & (1 << (BTN_9 % 8))) { DEBUG("BTN_9 "); }
			if (key_caps[BTN_SIDE/8] & (1 << (BTN_SIDE % 8))) { DEBUG("BTN_SIDE "); }
			if (key_caps[BTN_EXTRA/8] & (1 << (BTN_EXTRA % 8))) { DEBUG("BTN_EXTRA "); }
			if (key_caps[BTN_FORWARD/8] & (1 << (BTN_FORWARD % 8))) { DEBUG("BTN_FORWARD "); }
			if (key_caps[BTN_BACK/8] & (1 << (BTN_BACK % 8))) { DEBUG("BTN_BACK "); }
			if (key_caps[BTN_TASK/8] & (1 << (BTN_TASK % 8))) { DEBUG("BTN_TASK "); }
			if (key_caps[BTN_JOYSTICK/8] & (1 << (BTN_JOYSTICK % 8))) { DEBUG("BTN_JOYSTICK "); }
			if (key_caps[BTN_TRIGGER/8] & (1 << (BTN_TRIGGER % 8))) { DEBUG("BTN_TRIGGER "); }
			if (key_caps[BTN_THUMB/8] & (1 << (BTN_THUMB % 8))) { DEBUG("BTN_THUMB "); }
			if (key_caps[BTN_THUMB2/8] & (1 << (BTN_THUMB2 % 8))) { DEBUG("BTN_THUMB2 "); }
			if (key_caps[BTN_TOP/8] & (1 << (BTN_TOP % 8))) { DEBUG("BTN_TOP "); }
			if (key_caps[BTN_TOP2/8] & (1 << (BTN_TOP2 % 8))) { DEBUG("BTN_TOP2 "); }
			if (key_caps[BTN_PINKIE/8] & (1 << (BTN_PINKIE % 8))) { DEBUG("BTN_PINKIE "); }
			if (key_caps[BTN_BASE/8] & (1 << (BTN_BASE % 8))) { DEBUG("BTN_BASE "); }
			if (key_caps[BTN_BASE2/8] & (1 << (BTN_BASE2 % 8))) { DEBUG("BTN_BASE2 "); }
			if (key_caps[BTN_BASE3/8] & (1 << (BTN_BASE3 % 8))) { DEBUG("BTN_BASE3 "); }
			if (key_caps[BTN_BASE4/8] & (1 << (BTN_BASE4 % 8))) { DEBUG("BTN_BASE4 "); }
			if (key_caps[BTN_BASE5/8] & (1 << (BTN_BASE5 % 8))) { DEBUG("BTN_BASE5 "); }
			if (key_caps[BTN_BASE6/8] & (1 << (BTN_BASE6 % 8))) { DEBUG("BTN_BASE6 "); }
			if (key_caps[BTN_DEAD/8] & (1 << (BTN_DEAD % 8))) { DEBUG("BTN_DEAD "); }
			if (key_caps[BTN_GAMEPAD/8] & (1 << (BTN_GAMEPAD % 8))) { DEBUG("BTN_GAMEPAD "); }
			if (key_caps[BTN_SOUTH/8] & (1 << (BTN_SOUTH % 8))) { DEBUG("BTN_SOUTH "); }
			if (key_caps[BTN_A/8] & (1 << (BTN_A % 8))) { DEBUG("BTN_A "); }
			if (key_caps[BTN_EAST/8] & (1 << (BTN_EAST % 8))) { DEBUG("BTN_EAST "); }
			if (key_caps[BTN_B/8] & (1 << (BTN_B % 8))) { DEBUG("BTN_B "); }
			if (key_caps[BTN_C/8] & (1 << (BTN_C % 8))) { DEBUG("BTN_C "); }
			if (key_caps[BTN_NORTH/8] & (1 << (BTN_NORTH % 8))) { DEBUG("BTN_NORTH "); }
			if (key_caps[BTN_X/8] & (1 << (BTN_X % 8))) { DEBUG("BTN_X "); }
			if (key_caps[BTN_WEST/8] & (1 << (BTN_WEST % 8))) { DEBUG("BTN_WEST "); }
			if (key_caps[BTN_Y/8] & (1 << (BTN_Y % 8))) { DEBUG("BTN_Y "); }
			if (key_caps[BTN_Z/8] & (1 << (BTN_Z % 8))) { DEBUG("BTN_Z "); }
			if (key_caps[BTN_TL/8] & (1 << (BTN_TL % 8))) { DEBUG("BTN_TL "); }
			if (key_caps[BTN_TR/8] & (1 << (BTN_TR % 8))) { DEBUG("BTN_TR "); }
			if (key_caps[BTN_TL2/8] & (1 << (BTN_TL2 % 8))) { DEBUG("BTN_TL2 "); }
			if (key_caps[BTN_TR2/8] & (1 << (BTN_TR2 % 8))) { DEBUG("BTN_TR2 "); }
			if (key_caps[BTN_SELECT/8] & (1 << (BTN_SELECT % 8))) { DEBUG("BTN_SELECT "); }
			if (key_caps[BTN_START/8] & (1 << (BTN_START % 8))) { DEBUG("BTN_START "); }
			if (key_caps[BTN_MODE/8] & (1 << (BTN_MODE % 8))) { DEBUG("BTN_MODE "); }
			if (key_caps[BTN_THUMBL/8] & (1 << (BTN_THUMBL % 8))) { DEBUG("BTN_THUMBL "); }
			if (key_caps[BTN_THUMBR/8] & (1 << (BTN_THUMBR % 8))) { DEBUG("BTN_THUMBR "); }
			if (key_caps[BTN_DIGI/8] & (1 << (BTN_DIGI % 8))) { DEBUG("BTN_DIGI "); }
			if (key_caps[BTN_TOOL_PEN/8] & (1 << (BTN_TOOL_PEN % 8))) { DEBUG("BTN_TOOL_PEN "); }
			if (key_caps[BTN_TOOL_RUBBER/8] & (1 << (BTN_TOOL_RUBBER % 8))) { DEBUG("BTN_TOOL_RUBBER "); }
			if (key_caps[BTN_TOOL_BRUSH/8] & (1 << (BTN_TOOL_BRUSH % 8))) { DEBUG("BTN_TOOL_BRUSH "); }
			if (key_caps[BTN_TOOL_PENCIL/8] & (1 << (BTN_TOOL_PENCIL % 8))) { DEBUG("BTN_TOOL_PENCIL "); }
			if (key_caps[BTN_TOOL_AIRBRUSH/8] & (1 << (BTN_TOOL_AIRBRUSH % 8))) { DEBUG("BTN_TOOL_AIRBRUSH "); }
			if (key_caps[BTN_TOOL_FINGER/8] & (1 << (BTN_TOOL_FINGER % 8))) { DEBUG("BTN_TOOL_FINGER "); }
			if (key_caps[BTN_TOOL_MOUSE/8] & (1 << (BTN_TOOL_MOUSE % 8))) { DEBUG("BTN_TOOL_MOUSE "); }
			if (key_caps[BTN_TOOL_LENS/8] & (1 << (BTN_TOOL_LENS % 8))) { DEBUG("BTN_TOOL_LENS "); }
			if (key_caps[BTN_TOOL_QUINTTAP/8] & (1 << (BTN_TOOL_QUINTTAP % 8))) { DEBUG("BTN_TOOL_QUINTTAP "); }
			if (key_caps[BTN_STYLUS3/8] & (1 << (BTN_STYLUS3 % 8))) { DEBUG("BTN_STYLUS3 "); }
			if (key_caps[BTN_TOUCH/8] & (1 << (BTN_TOUCH % 8))) { DEBUG("BTN_TOUCH "); }
			if (key_caps[BTN_STYLUS/8] & (1 << (BTN_STYLUS % 8))) { DEBUG("BTN_STYLUS "); }
			if (key_caps[BTN_STYLUS2/8] & (1 << (BTN_STYLUS2 % 8))) { DEBUG("BTN_STYLUS2 "); }
			if (key_caps[BTN_TOOL_DOUBLETAP/8] & (1 << (BTN_TOOL_DOUBLETAP % 8))) { DEBUG("BTN_TOOL_DOUBLETAP "); }
			if (key_caps[BTN_TOOL_TRIPLETAP/8] & (1 << (BTN_TOOL_TRIPLETAP % 8))) { DEBUG("BTN_TOOL_TRIPLETAP "); }
			if (key_caps[BTN_TOOL_QUADTAP/8] & (1 << (BTN_TOOL_QUADTAP % 8))) { DEBUG("BTN_TOOL_QUADTAP "); }
			if (key_caps[BTN_GEAR_DOWN/8] & (1 << (BTN_GEAR_DOWN % 8))) { DEBUG("BTN_GEAR_DOWN "); }
			if (key_caps[BTN_GEAR_UP/8] & (1 << (BTN_GEAR_UP % 8))) { DEBUG("BTN_GEAR_UP "); }

			if (key_caps[KEY_POWER/8] & (1 << (KEY_POWER % 8))) { DEBUG("KEY_POWER "); }    // KEY_POWER  (note: causes PC to sleep)
			if (key_caps[KEY_SLEEP/8] & (1 << (KEY_SLEEP % 8))) { DEBUG("KEY_SLEEP "); }
			if (key_caps[KEY_SEARCH/8] & (1 << (KEY_SEARCH % 8))) { DEBUG("KEY_SEARCH "); } // KEY_SEARCH (Alexa button)

			if (key_caps[KEY_UP/8] & (1 << (KEY_UP % 8))) { DEBUG("KEY_UP "); }
			if (key_caps[KEY_DOWN/8] & (1 << (KEY_DOWN % 8))) { DEBUG("KEY_DOWN "); }
			if (key_caps[KEY_LEFT/8] & (1 << (KEY_LEFT % 8))) { DEBUG("KEY_LEFT "); }
			if (key_caps[KEY_RIGHT/8] & (1 << (KEY_RIGHT % 8))) { DEBUG("KEY_RIGHT "); }
			if (key_caps[KEY_ENTER/8] & (1 << (KEY_ENTER % 8))) { DEBUG("KEY_ENTER "); }
			if (key_caps[KEY_KPENTER/8] & (1 << (KEY_KPENTER % 8))) { DEBUG("KEY_KPENTER "); }

			if (key_caps[KEY_BACK/8] & (1 << (KEY_BACK % 8))) { DEBUG("KEY_BACK "); }
			if (key_caps[KEY_HOMEPAGE/8] & (1 << (KEY_HOMEPAGE % 8))) { DEBUG("KEY_HOMEPAGE "); }
			if (key_caps[KEY_MENU/8] & (1 << (KEY_MENU % 8))) { DEBUG("KEY_MENU "); }

			if (key_caps[KEY_REWIND/8] & (1 << (KEY_REWIND % 8))) { DEBUG("KEY_REWIND "); }
			if (key_caps[KEY_PLAYPAUSE/8] & (1 << (KEY_PLAYPAUSE % 8))) { DEBUG("KEY_PLAYPAUSE "); }
			if (key_caps[KEY_FASTFORWARD/8] & (1 << (KEY_FASTFORWARD % 8))) { DEBUG("KEY_FASTFORWARD "); }

			if (key_caps[KEY_MUTE/8] & (1 << (KEY_MUTE % 8))) { DEBUG("KEY_MUTE "); }
			if (key_caps[KEY_VOLUMEUP/8] & (1 << (KEY_VOLUMEUP % 8))) { DEBUG("KEY_VOLUMEUP "); }
			if (key_caps[KEY_VOLUMEDOWN/8] & (1 << (KEY_VOLUMEDOWN % 8))) { DEBUG("KEY_VOLUMEDOWN "); }
			if (key_caps[KEY_PROGRAM/8] & (1 << (KEY_PROGRAM % 8))) { DEBUG("KEY_PROGRAM "); }

			if (key_caps[KEY_DISPLAY_OFF/8] & (1 << (KEY_DISPLAY_OFF % 8))) { DEBUG("KEY_DISPLAY_OFF "); }
			if (key_caps[KEY_DISPLAYTOGGLE/8] & (1 << (KEY_DISPLAYTOGGLE % 8))) { DEBUG("KEY_DISPLAYTOGGLE "); }
			if (key_caps[KEY_SWITCHVIDEOMODE/8] & (1 << (KEY_SWITCHVIDEOMODE % 8))) { DEBUG("KEY_SWITCHVIDEOMODE "); }
			if (key_caps[KEY_VIDEO_NEXT/8] & (1 << (KEY_VIDEO_NEXT % 8))) { DEBUG("KEY_VIDEO_NEXT "); }
			if (key_caps[KEY_VIDEO_PREV/8] & (1 << (KEY_VIDEO_PREV % 8))) { DEBUG("KEY_VIDEO_PREV "); }
			DEBUG("\n");
			// TODO: add additional keys
		}
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get supported relative axes
	// (EV_REL, REL_MAX)
	//////////////////////////////////////////////////
	if (etypes & (1 << EV_REL)) {
		unsigned char rel_caps[(REL_MAX / 8) + 1];
		std::memset(rel_caps, 0, sizeof(rel_caps));
		DEBUG("supported relative axes:\n");

		if (ioctl(fd, EVIOCGBIT(EV_REL, sizeof(rel_caps)), &rel_caps) != -1) {
			[[maybe_unused]] uint8_t * ptr = (uint8_t *)&rel_caps[(REL_MAX / 8) + 1];
			for (size_t i = sizeof(rel_caps); ; i -= sizeof(uint8_t)) {
				DEBUG("%x ", *ptr);
				ptr--;
				if (i == 0) { break; }
				// 1f 10 40
			}
			DEBUG("\n");

			if (rel_caps[REL_X/8] & (1 << (REL_X % 8))) { DEBUG("REL_X "); }
			if (rel_caps[REL_Y/8] & (1 << (REL_Y % 8))) { DEBUG("REL_Y "); }
			if (rel_caps[REL_Z/8] & (1 << (REL_Z % 8))) { DEBUG("REL_Z "); }
			if (rel_caps[REL_RX/8] & (1 << (REL_RX % 8))) { DEBUG("REL_RX "); }
			if (rel_caps[REL_RY/8] & (1 << (REL_RY % 8))) { DEBUG("REL_RY "); }
			if (rel_caps[REL_RZ/8] & (1 << (REL_RZ % 8))) { DEBUG("REL_RZ "); }
			if (rel_caps[REL_WHEEL/8] & (1 << (REL_WHEEL % 8))) { DEBUG("REL_WHEEL "); }
			if (rel_caps[REL_WHEEL_HI_RES/8] & (1 << (REL_WHEEL_HI_RES % 8))) { DEBUG("REL_WHEEL_HI_RES "); }
			if (rel_caps[REL_HWHEEL/8] & (1 << (REL_HWHEEL % 8))) { DEBUG("REL_HWHEEL "); }
			if (rel_caps[REL_HWHEEL_HI_RES/8] & (1 << (REL_HWHEEL_HI_RES % 8))) { DEBUG("REL_HWHEEL_HI_RES "); }
			if (rel_caps[REL_DIAL/8] & (1 << (REL_DIAL % 8))) { DEBUG("REL_DIAL "); }
			if (rel_caps[REL_MISC/8] & (1 << (REL_MISC % 8))) { DEBUG("REL_MISC "); }
			DEBUG("\n");
		}
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get supported absolute axes
	// (EV_ABS, ABS_MAX)
	//////////////////////////////////////////////////
	if (etypes & (1 << EV_ABS)) {
		unsigned char abs_caps[(ABS_MAX / 8) + 1];
		std::memset(abs_caps, 0, sizeof(abs_caps));
		DEBUG("supported absolute axes:\n");

		if (ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(abs_caps)), &abs_caps) != -1) {
			[[maybe_unused]] uint8_t * ptr = (uint8_t *)&abs_caps[(ABS_MAX / 8) + 1];
			for (size_t i = sizeof(abs_caps); ; i -= sizeof(uint8_t)) {
				DEBUG("%x ", *ptr);
				ptr--;
				if (i == 0) { break; }
			}
			DEBUG("\n");

			if (abs_caps[ABS_X/8] & (1 << (ABS_X % 8))) { DEBUG("ABS_X "); }
			if (abs_caps[ABS_Y/8] & (1 << (ABS_Y % 8))) { DEBUG("ABS_Y "); }
			if (abs_caps[ABS_Z/8] & (1 << (ABS_Z % 8))) { DEBUG("ABS_Z "); }
			if (abs_caps[ABS_RX/8] & (1 << (ABS_RX % 8))) { DEBUG("ABS_RX "); }
			if (abs_caps[ABS_RY/8] & (1 << (ABS_RY % 8))) { DEBUG("ABS_RY "); }
			if (abs_caps[ABS_RZ/8] & (1 << (ABS_RZ % 8))) { DEBUG("ABS_RZ "); }
			if (abs_caps[ABS_MISC/8] & (1 << (ABS_MISC % 8))) { DEBUG("ABS_MISC "); }
			if (abs_caps[ABS_VOLUME/8] & (1 << (ABS_VOLUME % 8))) { DEBUG("ABS_VOLUME "); }
			if (abs_caps[ABS_GAS/8] & (1 << (ABS_GAS % 8))) { DEBUG("ABS_GAS "); }
			if (abs_caps[ABS_THROTTLE/8] & (1 << (ABS_THROTTLE % 8))) { DEBUG("ABS_THROTTLE "); }
			if (abs_caps[ABS_BRAKE/8] & (1 << (ABS_BRAKE % 8))) { DEBUG("ABS_BRAKE "); }
			if (abs_caps[ABS_WHEEL/8] & (1 << (ABS_WHEEL % 8))) { DEBUG("ABS_WHEEL "); }
			if (abs_caps[ABS_DISTANCE/8] & (1 << (ABS_DISTANCE % 8))) { DEBUG("ABS_DISTANCE "); }
			if (abs_caps[ABS_PRESSURE/8] & (1 << (ABS_PRESSURE % 8))) { DEBUG("ABS_PRESSURE "); }
			if (abs_caps[ABS_PROFILE/8] & (1 << (ABS_PROFILE % 8))) { DEBUG("ABS_PROFILE "); }
			if (abs_caps[ABS_RUDDER/8] & (1 << (ABS_RUDDER % 8))) { DEBUG("ABS_RUDDER "); }
			if (abs_caps[ABS_TILT_X/8] & (1 << (ABS_TILT_X % 8))) { DEBUG("ABS_TILT_X "); }
			if (abs_caps[ABS_TILT_Y/8] & (1 << (ABS_TILT_Y % 8))) { DEBUG("ABS_TILT_Y "); }
			if (abs_caps[ABS_TOOL_WIDTH/8] & (1 << (ABS_TOOL_WIDTH % 8))) { DEBUG("ABS_TOOL_WIDTH "); }
			if (abs_caps[ABS_HAT0X/8] & (1 << (ABS_HAT0X % 8))) { DEBUG("ABS_HAT0X "); }
			if (abs_caps[ABS_HAT0Y/8] & (1 << (ABS_HAT0Y % 8))) { DEBUG("ABS_HAT0Y "); }
			if (abs_caps[ABS_HAT1X/8] & (1 << (ABS_HAT1X % 8))) { DEBUG("ABS_HAT1X "); }
			if (abs_caps[ABS_HAT1Y/8] & (1 << (ABS_HAT1Y % 8))) { DEBUG("ABS_HAT1Y "); }
			if (abs_caps[ABS_HAT2X/8] & (1 << (ABS_HAT2X % 8))) { DEBUG("ABS_HAT2X "); }
			if (abs_caps[ABS_HAT2Y/8] & (1 << (ABS_HAT2Y % 8))) { DEBUG("ABS_HAT2Y "); }
			if (abs_caps[ABS_HAT3X/8] & (1 << (ABS_HAT3X % 8))) { DEBUG("ABS_HAT3X "); }
			if (abs_caps[ABS_HAT3Y/8] & (1 << (ABS_HAT3Y % 8))) { DEBUG("ABS_HAT3Y "); }
			if (abs_caps[ABS_MT_BLOB_ID/8] & (1 << (ABS_MT_BLOB_ID % 8))) { DEBUG("ABS_MT_BLOB_ID "); }
			if (abs_caps[ABS_MT_DISTANCE/8] & (1 << (ABS_MT_DISTANCE % 8))) { DEBUG("ABS_MT_DISTANCE "); }
			if (abs_caps[ABS_MT_ORIENTATION/8] & (1 << (ABS_MT_ORIENTATION % 8))) { DEBUG("ABS_MT_ORIENTATION "); }
			if (abs_caps[ABS_MT_POSITION_X/8] & (1 << (ABS_MT_POSITION_X % 8))) { DEBUG("ABS_MT_POSITION_X "); }
			if (abs_caps[ABS_MT_POSITION_Y/8] & (1 << (ABS_MT_POSITION_Y % 8))) { DEBUG("ABS_MT_POSITION_Y "); }
			if (abs_caps[ABS_MT_PRESSURE/8] & (1 << (ABS_MT_PRESSURE % 8))) { DEBUG("ABS_MT_PRESSURE "); }
			if (abs_caps[ABS_MT_SLOT/8] & (1 << (ABS_MT_SLOT % 8))) { DEBUG("ABS_MT_SLOT "); }
			if (abs_caps[ABS_MT_TOOL_TYPE/8] & (1 << (ABS_MT_TOOL_TYPE % 8))) { DEBUG("ABS_MT_TOOL_TYPE "); }
			if (abs_caps[ABS_MT_TOOL_X/8] & (1 << (ABS_MT_TOOL_X % 8))) { DEBUG("ABS_MT_TOOL_X "); }
			if (abs_caps[ABS_MT_TOOL_Y/8] & (1 << (ABS_MT_TOOL_Y % 8))) { DEBUG("ABS_MT_TOOL_Y "); }
			if (abs_caps[ABS_MT_TOUCH_MAJOR/8] & (1 << (ABS_MT_TOUCH_MAJOR % 8))) { DEBUG("ABS_MT_TOUCH_MAJOR "); }
			if (abs_caps[ABS_MT_TOUCH_MINOR/8] & (1 << (ABS_MT_TOUCH_MINOR % 8))) { DEBUG("ABS_MT_TOUCH_MINOR "); }
			if (abs_caps[ABS_MT_TRACKING_ID/8] & (1 << (ABS_MT_TRACKING_ID % 8))) { DEBUG("ABS_MT_TRACKING_ID "); }
			if (abs_caps[ABS_MT_WIDTH_MAJOR/8] & (1 << (ABS_MT_WIDTH_MAJOR % 8))) { DEBUG("ABS_MT_WIDTH_MAJOR "); }
			if (abs_caps[ABS_MT_WIDTH_MAJOR/8] & (1 << (ABS_MT_WIDTH_MAJOR % 8))) { DEBUG("ABS_MT_WIDTH_MAJOR "); }
			DEBUG("\n");
		}
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get supported misc
	// (EV_MSC, MSC_MAX)
	//////////////////////////////////////////////////
	if (etypes & (1 << EV_MSC)) {
		unsigned char msc_caps[(MSC_MAX / 8) + 1];
		std::memset(msc_caps, 0, sizeof(msc_caps));
		DEBUG("supported misc:\n");

		if (ioctl(fd, EVIOCGBIT(EV_MSC, sizeof(msc_caps)), &msc_caps) != -1) {
			[[maybe_unused]] uint8_t * ptr = (uint8_t *)&msc_caps[(MSC_MAX / 8) + 1];
			for (size_t i = sizeof(msc_caps); ; i -= sizeof(uint8_t)) {
				DEBUG("%x ", *ptr);
				ptr--;
				if (i == 0) { break; }
			}
			DEBUG("\n");

			if (msc_caps[MSC_SERIAL/8] & (1 << (MSC_SERIAL % 8))) { DEBUG("MSC_SERIAL "); }
			if (msc_caps[MSC_PULSELED/8] & (1 << (MSC_PULSELED % 8))) { DEBUG("MSC_PULSELED "); }
			if (msc_caps[MSC_GESTURE/8] & (1 << (MSC_GESTURE % 8))) { DEBUG("MSC_GESTURE "); }
			if (msc_caps[MSC_RAW/8] & (1 << (MSC_RAW % 8))) { DEBUG("MSC_RAW "); }
			if (msc_caps[MSC_SCAN/8] & (1 << (MSC_SCAN % 8))) { DEBUG("MSC_SCAN "); }
			if (msc_caps[MSC_TIMESTAMP/8] & (1 << (MSC_TIMESTAMP % 8))) { DEBUG("MSC_TIMESTAMP "); }
			DEBUG("\n");
		}
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get supported switch
	// (EV_SW, SW_MAX)
	//////////////////////////////////////////////////
	if (etypes & (1 << EV_SW)) {
		unsigned char sw_caps[(SW_MAX / 8) + 1];
		std::memset(sw_caps, 0, sizeof(sw_caps));
		DEBUG("supported switch:\n");

		if (ioctl(fd, EVIOCGBIT(EV_SW, sizeof(sw_caps)), &sw_caps) != -1) {
			[[maybe_unused]] uint8_t * ptr = (uint8_t *)&sw_caps[(SW_MAX / 8) + 1];
			for (size_t i = sizeof(sw_caps); ; i -= sizeof(uint8_t)) {
				DEBUG("%x ", *ptr);
				ptr--;
				if (i == 0) { break; }
			}
			DEBUG("\n");

			if (sw_caps[SW_LID/8] & (1 << (SW_LID % 8))) { DEBUG("SW_LID "); }
			if (sw_caps[SW_TABLET_MODE/8] & (1 << (SW_TABLET_MODE % 8))) { DEBUG("SW_TABLET_MODE "); }
			if (sw_caps[SW_HEADPHONE_INSERT/8] & (1 << (SW_HEADPHONE_INSERT % 8))) { DEBUG("SW_HEADPHONE_INSERT "); }
			if (sw_caps[SW_RFKILL_ALL/8] & (1 << (SW_RFKILL_ALL % 8))) { DEBUG("SW_RFKILL_ALL "); }
			if (sw_caps[SW_MICROPHONE_INSERT/8] & (1 << (SW_MICROPHONE_INSERT % 8))) { DEBUG("SW_MICROPHONE_INSERT "); }
			if (sw_caps[SW_DOCK/8] & (1 << (SW_DOCK % 8))) { DEBUG("SW_DOCK "); }
			if (sw_caps[SW_LINEOUT_INSERT/8] & (1 << (SW_LINEOUT_INSERT % 8))) { DEBUG("SW_LINEOUT_INSERT "); }
			if (sw_caps[SW_JACK_PHYSICAL_INSERT/8] & (1 << (SW_JACK_PHYSICAL_INSERT % 8))) { DEBUG("SW_JACK_PHYSICAL_INSERT "); }
			if (sw_caps[SW_VIDEOOUT_INSERT/8] & (1 << (SW_VIDEOOUT_INSERT % 8))) { DEBUG("SW_VIDEOOUT_INSERT "); }
			if (sw_caps[SW_CAMERA_LENS_COVER/8] & (1 << (SW_CAMERA_LENS_COVER % 8))) { DEBUG("SW_CAMERA_LENS_COVER "); }
			if (sw_caps[SW_KEYPAD_SLIDE/8] & (1 << (SW_KEYPAD_SLIDE % 8))) { DEBUG("SW_KEYPAD_SLIDE "); }
			if (sw_caps[SW_FRONT_PROXIMITY/8] & (1 << (SW_FRONT_PROXIMITY % 8))) { DEBUG("SW_FRONT_PROXIMITY "); }
			if (sw_caps[SW_ROTATE_LOCK/8] & (1 << (SW_ROTATE_LOCK % 8))) { DEBUG("SW_ROTATE_LOCK "); }
			if (sw_caps[SW_LINEIN_INSERT/8] & (1 << (SW_LINEIN_INSERT % 8))) { DEBUG("SW_LINEIN_INSERT "); }
			if (sw_caps[SW_MUTE_DEVICE/8] & (1 << (SW_MUTE_DEVICE % 8))) { DEBUG("SW_MUTE_DEVICE "); }
			if (sw_caps[SW_PEN_INSERTED/8] & (1 << (SW_PEN_INSERTED % 8))) { DEBUG("SW_PEN_INSERTED "); }
			if (sw_caps[SW_MACHINE_COVER/8] & (1 << (SW_MACHINE_COVER % 8))) { DEBUG("SW_MACHINE_COVER "); }
			DEBUG("\n");
		}
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get supported led's
	// (EV_LED, LED_MAX)
	//////////////////////////////////////////////////
	if (etypes & (1 << EV_LED)) {
		unsigned char led_caps[(LED_MAX / 8) + 1];
		std::memset(led_caps, 0, sizeof(led_caps));
		DEBUG("supported LEDs:\n");

		if (ioctl(fd, EVIOCGBIT(EV_LED, sizeof(led_caps)), &led_caps) != -1) {
			[[maybe_unused]] uint8_t * ptr = (uint8_t *)&led_caps[(REL_MAX / 8) + 1];
			for (size_t i = sizeof(led_caps); ; i -= sizeof(uint8_t)) {
				DEBUG("%x ", *ptr);
				ptr--;
				if (i == 0) { break; }
			}
			DEBUG("\n");

			if (led_caps[LED_CAPSL/8] & (1 << (LED_CAPSL % 8))) { DEBUG("LED_CAPSL "); }
			if (led_caps[LED_CHARGING/8] & (1 << (REL_Y % 8))) { DEBUG("LED_CHARGING "); }
			if (led_caps[LED_COMPOSE/8] & (1 << (LED_COMPOSE % 8))) { DEBUG("LED_COMPOSE "); }
			if (led_caps[LED_KANA/8] & (1 << (LED_KANA % 8))) { DEBUG("LED_KANA "); }
			if (led_caps[LED_MAIL/8] & (1 << (LED_MAIL % 8))) { DEBUG("LED_MAIL "); }
			if (led_caps[LED_MISC/8] & (1 << (LED_MISC % 8))) { DEBUG("LED_MISC "); }
			if (led_caps[LED_MUTE/8] & (1 << (LED_MUTE % 8))) { DEBUG("LED_MUTE "); }
			if (led_caps[LED_NUML/8] & (1 << (LED_NUML % 8))) { DEBUG("LED_NUML "); }
			if (led_caps[LED_SCROLLL/8] & (1 << (LED_SCROLLL % 8))) { DEBUG("LED_SCROLLL "); }
			if (led_caps[LED_SLEEP/8] & (1 << (LED_SLEEP % 8))) { DEBUG("LED_SLEEP "); }
			if (led_caps[LED_SUSPEND/8] & (1 << (LED_SUSPEND % 8))) { DEBUG("LED_SUSPEND "); }
			DEBUG("\n");
		}
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get supported sounds
	// (EV_SND, SND_MAX)
	//////////////////////////////////////////////////
	if (etypes & (1 << EV_SND)) {
		unsigned char snd_caps[(MSC_MAX / 8) + 1];
		std::memset(snd_caps, 0, sizeof(snd_caps));
		DEBUG("supported sounds:\n");

		if (ioctl(fd, EVIOCGBIT(EV_SND, sizeof(snd_caps)), &snd_caps) != -1) {
			[[maybe_unused]] uint8_t * ptr = (uint8_t *)&snd_caps[(SND_MAX / 8) + 1];
			for (size_t i = sizeof(snd_caps); ; i -= sizeof(uint8_t)) {
				DEBUG("%x ", *ptr);
				ptr--;
				if (i == 0) { break; }
			}
			DEBUG("\n");

			if (snd_caps[SND_CLICK/8] & (1 << (SND_CLICK % 8))) { DEBUG("SND_CLICK "); }
			if (snd_caps[SND_BELL/8] & (1 << (SND_BELL % 8))) { DEBUG("SND_BELL "); }
			if (snd_caps[SND_TONE/8] & (1 << (SND_TONE % 8))) { DEBUG("SND_TONE "); }
			DEBUG("\n");
		}
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// get supported autorepeat values
	// (EV_REP, REP_MAX)
	//////////////////////////////////////////////////
	if (etypes & (1 << EV_REP)) {
		DEBUG("supported repeat:\n");
		// get current values
		unsigned int rep[2] = { 0, 0 };
		ioctl(fd, EVIOCGREP, rep);
		DEBUG("delay:%i, period:%i\n", rep[REP_DELAY], rep[REP_PERIOD]);
		// set new values
		//ioctl(fd, EVIOCSREP, rep)
	}
	//////////////////////////////////////////////////

	DEBUG("\n------------------------------\n");

	close(fd);
	return true;
}

/* get supported keys */
bool Devices::isKeySupported(const char* device, int key) {
	int fd = open(device, O_RDONLY);
	if (fd == -1) {
		DEBUG("%s - failed to open device: %s (%s)\n", __func__, device, strerror(errno));
		return false;
	}
	bool r = isKeySupported(fd, key);
	close(fd);
	return r;
}

/* get supported keys */
bool Devices::isKeySupported(int fd, int key) {

	unsigned char keys[(KEY_MAX / 8) + 1];
	std::memset(keys, 0, sizeof(keys));

	// get supported 'Keys and buttons'
	int r = ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keys)), &keys);
	if (r == -1) {
		DEBUG("%s - io error fd: %i\n", __func__, fd);
		return false;
	}
	return (keys[key/8] & (1 << (key % 8)));
}

/* get supported relative axes */
bool Devices::isRelSupported(const char* device, int rel) {
	int fd = open(device, O_RDONLY);
	if (fd == -1) {
		DEBUG("%s - failed to open device: %s (%s)\n", __func__, device, strerror(errno));
		return false;
	}
	bool r = isRelSupported(fd, rel);
	close(fd);
	return r;
}

/* get supported relative axes */
bool Devices::isRelSupported(int fd, int rel) {

	unsigned char rel_caps[(REL_MAX / 8) + 1];
	std::memset(rel_caps, 0, sizeof(rel_caps));

	if (ioctl(fd, EVIOCGBIT(EV_REL, sizeof(rel_caps)), &rel_caps) != -1) {
		//if (rel_caps[REL_X/8] & (1 << (REL_X % 8))) { DEBUG("REL_X "); }
		//if (rel_caps[REL_Y/8] & (1 << (REL_Y % 8))) { DEBUG("REL_Y "); }
		//if (rel_caps[REL_Z/8] & (1 << (REL_Z % 8))) { DEBUG("REL_Z "); }
		//if (rel_caps[REL_RX/8] & (1 << (REL_RX % 8))) { DEBUG("REL_RX "); }
		//if (rel_caps[REL_RY/8] & (1 << (REL_RY % 8))) { DEBUG("REL_RY "); }
		//if (rel_caps[REL_RZ/8] & (1 << (REL_RZ % 8))) { DEBUG("REL_RZ "); }
		//if (rel_caps[REL_WHEEL/8] & (1 << (REL_WHEEL % 8))) { DEBUG("REL_WHEEL "); }
		//if (rel_caps[REL_WHEEL_HI_RES/8] & (1 << (REL_WHEEL_HI_RES % 8))) { DEBUG("REL_WHEEL_HI_RES "); }
		//if (rel_caps[REL_HWHEEL/8] & (1 << (REL_HWHEEL % 8))) { DEBUG("REL_HWHEEL "); }
		//if (rel_caps[REL_HWHEEL_HI_RES/8] & (1 << (REL_HWHEEL_HI_RES % 8))) { DEBUG("REL_HWHEEL_HI_RES "); }
		//if (rel_caps[REL_DIAL/8] & (1 << (REL_DIAL % 8))) { DEBUG("REL_DIAL "); }
		//if (rel_caps[REL_MISC/8] & (1 << (REL_MISC % 8))) { DEBUG("REL_MISC "); }

		if (rel_caps[rel/8] & (1 << (rel % 8))) { return true; }
	}
	return false;
}
