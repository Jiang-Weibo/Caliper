/// @file ContextRecord.h
/// ContextRecord class definition

#ifndef CALI_CONTEXTRECORD_H
#define CALI_CONTEXTRECORD_H

#include "cali_types.h"

#include "Attribute.h"

#include <functional>
#include <vector>
#include <memory>

namespace cali
{

class Node;
class Query;

class ContextRecord
{

public:

    static 
    std::vector< std::unique_ptr<cali::Query> > 
    unpack(std::function<cali::Attribute  (ctx_id_t)> get_attr, 
           std::function<const cali::Node*(ctx_id_t)> get_node,
           const uint64_t buf[], size_t size);
};

}

#endif