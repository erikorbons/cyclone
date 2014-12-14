#ifndef CYCLONE_LANG_SLOT_H__
#define CYCLONE_LANG_SLOT_H__

#include "NamedObject.h"

namespace cyclone {
namespace lang {

class Slot : public NamedObject {
public:

	virtual ~Slot () { }
};

}	// namespace lang
}	// namespace cyclone

#endif	// CYCLONE_LANG_SLOT_H__
