#ifndef CYCLONE_LANG_SCOPEDOBJECT_H__
#define CYCLONE_LANG_SCOPEDOBJECT_H__

namespace cyclone {
namespace lang {

/**
 * Scoped objects exist within a module and have a namespace, but no name.
 * The NamedObject specialisation als has a name.
 */
class ScopedObject {
public:

	virtual ~ModuleObject () const { }

	virtual const Module & module () const = 0;

	virtual const std::u16string & namespaceName () const = 0;

	virtual bool isNamedObject () const = 0;
	virtual bool isNamedType () const = 0;
	virtual bool isFunction () const = 0;	// Functions are named objects
	virtual bool isSlot () const = 0;	// Variables, constants. Are named objects.
	virtual bool isTraitImplementation () const = 0;	// Link between a type and a trait, not named (the type and trait are named).
};

}	// lang
}	// cyclone

#endif	// CYCLONE_LANG_MODULEOBJECT_H__
