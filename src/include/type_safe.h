#ifndef TYPE_SAFE_H
#define TYPE_SAFE_H

#include <memory>
#include <string>


using StringShr=std::shared_ptr<std::string>;
using StringUnq=std::unique_ptr<std::string>;

#endif
