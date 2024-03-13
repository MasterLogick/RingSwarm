#ifndef RINGSWARM_SETTINGS_H
#define RINGSWARM_SETTINGS_H

#include <string>

namespace RingSwarm::core {
void setSetting(const char *name, const char *value);

std::string getSetting(const char *name);
}// namespace RingSwarm::core

#endif//RINGSWARM_SETTINGS_H
