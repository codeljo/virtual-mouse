#pragma once
#ifndef EVENT_DESC_H
#define EVENT_DESC_H

#include <cstdint>

const char* getEventTypeDescription(uint16_t type);
const char* getEventCodeDescription(uint16_t type, uint16_t code);

#endif  /* EVENT_DESC_H */