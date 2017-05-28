#include <string>

#include "state.h"

namespace aton {

template<typename T>
State<T>::State(const T& trigger)
    : trigger_(trigger) {
  fingerprint_ = std::hash<T>(trigger);
}

}
