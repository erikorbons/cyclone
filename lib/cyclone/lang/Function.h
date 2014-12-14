#ifndef CYCLONE_LANG_FUNCTION_H__
#define CYCLONE_LANG_FUNCTION_H__

#include "NamedObject.h"

namespace cyclone {
namespace lang {

class Function : public NamedObject {
public:

	virtual ~Function () { }
};

}	// lang
}	// cyclone

#endif	// CYCLONE_LANG_FUNCTION_H__
