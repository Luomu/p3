#include <entityx/entityx.h>

namespace p3 {
template <class T>
using ent_ptr = entityx::ptr<T>;

using entityx::EntityManager;
using entityx::EventManager;
}
