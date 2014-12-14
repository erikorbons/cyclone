#ifndef CYCLONE_LANG_TYPE_H__
#define CYCLONE_LANG_TYPE_H__

namespace cyclone {
namespace lang {

/**
 * Type hierarchy:
 * + Type
 * | + GenericType
 * | |-+ FunctionType
 * | |-+ StructType
 * | |-+ TupleType
 * | |-+ AlgebraicType
 * | `-+ TraitType
 * |-+ EnumType
 * |-+ PrimitiveType
 * |-+ ArrayType
 * `-+ ReferenceType
 */
class Type {
public:

	virtual ~Type () { }
};

}	// namespace lang
}	// namespace cyclone

#endif	// CYCLONE_LANG_TYPE_H__
