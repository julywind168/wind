#ifndef WIND_TYPE_H
#define WIND_TYPE_H

#include "types/primitives.h"
#include "types/named.h"
#include "types/quote.h"
#include "types/boxed.h"


namespace wind {

std::shared_ptr<Type> getType(std::string name, std::vector<std::shared_ptr<Type>> parameters);

}


#endif
