#ifndef CYCLONE_LANG_MODULE_H__
#define CYCLONE_LANG_MODULE_H__

#include <string>

namespace cyclone {
namespace lang {

/**
 * A module contains:
 * - Values / constants:
 *   - named
 *   - typed
 * - Functions:
 *   - named
 *   - typed
 *   - callable
 * - Named types (structs, enums, tuples, traits, etc.):
 *   - named
 *   - typed
 * - Trait implementations: link between a type and a trait.
 */
class Module {
public:

	virtual ~Module () { }

	virtual const std::u16string & group () const = 0;
	virtual const std::u16string & name () const = 0;
	virtual const std::u16string & version () const = 0;
};

}
}

#endif	// CYCLONE_LANG_MODULE_H__
