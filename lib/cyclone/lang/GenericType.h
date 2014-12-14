#ifndef CYCLONE_LANG_GENERICTYPE_H__
#define CYCLONE_LANG_GENERICTYPE_H__

#include "Type.h"

namespace cyclone {
namespace lang {

class GenericType : public Type {
public:

	virtual ~GenericType () { }
};

}	// lang
}	// cyclone

#endif	// CYCLONE_LANG_GENERICTYPE_H__
