#ifndef CYCLONE_LANG_NAMEDTYPE_H__
#define CYCLONE_LANG_NAMEDTYPE_H__

#include "NamedObject.h"

namespace cyclone {
namespace lang {

class NamedType : public NamedObject {
public:

	virtual ~NamedType () { }
};

}
}
#endif	// CYCLONE_LANG_NAMEDTYPE_H__
