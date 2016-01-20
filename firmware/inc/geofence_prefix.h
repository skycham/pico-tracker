/**
 * Autogenerated prefix structures. See sim/geofence
 */

#ifndef GEOFENCE_PREFIX_H
#define GEOFENCE_PREFIX_H


#include <stdbool.h>

#include "samd20.h"


/**
 * struct representing all we need to know about a prefix
 */
struct prefix_t {
  const int32_t** outlines;
  const uint32_t outline_count;
  const uint32_t* outline_lengths;
  char* prefix;
};


const struct prefix_t prefixs[46];

#endif /* GEOFENCE_PREFIX_H */
