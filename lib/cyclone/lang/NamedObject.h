#ifndef CYCLONE_LANG_NAMEDOBJECT_H__
#define CYCLONE_LANG_NAMEDOBJECT_H__

#include <string>
#include "ScopedObject.h"

namespace cyclone {
namespace lang {

class Module;

/**
 * Named objects are contained in a module. They have a namespace and a name.
 */
class NamedObject : public ScopedObject {
public:

	virtual ~NamedObject () { }

	virtual const std::u16string & name () const = 0;
};

}	// namespace lang
}	// namespace cyclone

#endif	// CYCLONE_LANG_NAMEDOBJECT_H__
