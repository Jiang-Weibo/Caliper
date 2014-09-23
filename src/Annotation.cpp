///@ file Annotation.cpp
///@ Annotation interface template specializations


#include "Annotation.h"
#include "Caliper.h"

using namespace std;
using namespace cali;


// --- Constructors / destructor

Annotation::Annotation(const std::string& name, Option opt)
    : m_attr { Attribute::invalid }, m_name { name }, m_opt { opt }, m_depth { 0 }
{
    auto p = Caliper::instance()->get_attribute(name);

    if (p.first)
        m_attr = p.second;
}

Annotation::~Annotation() {
    while (!(m_opt & KeepAlive) && m_depth-- > 0)
        end();
}


// --- begin() overloads

ctx_err Annotation::begin(int data)
{
    uint64_t buf = static_cast<uint64_t>(data);
    return begin(CTX_TYPE_INT, &buf, sizeof(uint64_t));
}

ctx_err Annotation::begin(double data)
{
    return begin(CTX_TYPE_DOUBLE, &data, sizeof(double));
}

ctx_err Annotation::begin(const std::string& data)
{
    return begin(CTX_TYPE_STRING, data.data(), data.size());
}

ctx_err Annotation::begin(ctx_attr_type type, const void* data, size_t size)
{
    if (m_attr == Attribute::invalid)
        create_attribute(type);

    if (!m_attr.type() == type)
        return CTX_EINV;

    Caliper* c  = Caliper::instance();

    ctx_err ret = c->begin(c->current_environment(), m_attr, data, size);

    if (ret == CTX_SUCCESS)
        ++m_depth;

    return ret;
}


// --- set() overloads

ctx_err Annotation::set(int data)
{
    uint64_t buf = static_cast<uint64_t>(data);
    return set(CTX_TYPE_INT, &buf, sizeof(uint64_t));
}

ctx_err Annotation::set(double data)
{
    return set(CTX_TYPE_DOUBLE, &data, sizeof(double));
}

ctx_err Annotation::set(const std::string& data)
{
    return set(CTX_TYPE_STRING, data.c_str(), data.size());
}

ctx_err Annotation::set(ctx_attr_type type, const void* data, size_t size)
{
    if (m_attr == Attribute::invalid)
        create_attribute(type);

    if (!m_attr.type() == type)
        return CTX_EINV;

    Caliper* c  = Caliper::instance();

    ctx_err ret = c->set(c->current_environment(), m_attr, data, size);

    if (ret == CTX_SUCCESS)
        ++m_depth;

    return ret;
}


// --- end()

ctx_err Annotation::end()
{
    Caliper* c = Caliper::instance();

    ctx_err ret = c->end(c->current_environment(), m_attr);

    if (m_depth > 0)
        --m_depth;

    return ret;
}


// --- init_attribute 

void Annotation::create_attribute(ctx_attr_type type)
{
    // Option -> ctx_attr_properties map
    const int prop[] = {
        CTX_ATTR_DEFAULT,                   // Default      = 0
        CTX_ATTR_ASVALUE,                   // StoreAsValue = 1
        CTX_ATTR_NOMERGE,                   // NoMerge      = 2
        CTX_ATTR_ASVALUE | CTX_ATTR_NOMERGE // StoreAsValue | NoMerge = 3
    };

    m_attr = Caliper::instance()->create_attribute(m_name, type, prop[m_opt & 0x03]);
}