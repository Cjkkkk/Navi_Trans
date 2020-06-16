#include <pack.hpp>

using namespace navi_trans;

class B{
public:
    B() {}
    int32_t a;
    int32_t b;
    size_t do_pack_size() const {
        size_t payload_size = 0;
        payload_size += pack_size<INT_32>(generate_key<INT_32>(1));
        payload_size += pack_size<INT_32>(a);
        payload_size += pack_size<INT_32>(generate_key<INT_32>(2));
        payload_size += pack_size<INT_32>(b);
        return payload_size;
    };

    size_t do_pack(uint8_t* buf) const {
        size_t payload_size = 0;
        payload_size += pack_key<INT_32>(buf + payload_size, 1);
        payload_size += pack<INT_32>(buf + payload_size, a);
        payload_size += pack_key<INT_32>(buf + payload_size, 2);
        payload_size += pack<INT_32>(buf + payload_size, b);
        return payload_size;
    };

    size_t do_unpack(uint8_t* buf, size_t& payload_size){
        uint32_t field_number = 0;
        size_t offset = 0;
        offset += unpack_key<INT_32>(buf + offset, field_number);
        offset += unpack<INT_32>(buf + offset, a);
        offset += unpack_key<INT_32>(buf + offset, field_number);
        offset += unpack<INT_32>(buf + offset, b);
        return payload_size;
    };

};

class A{
public:
    A() {}
    int32_t a;
    Array<int32_t, INT_32> b;
    B c;
    size_t do_pack_size() const {
        size_t payload_size = 0;
        payload_size += pack_size<INT_32>(generate_key<INT_32>(1));
        payload_size += pack_size<INT_32>(a);
        payload_size += pack_size<INT_32>(generate_key<INT_32>(2));
        payload_size += pack_size<ARRAY>(b);
        payload_size += pack_size<INT_32>(generate_key<INT_32>(3));
        payload_size += pack_size<MESSAGE>(c);
        return payload_size;
    };

    size_t do_pack(uint8_t* buf) const {
        size_t payload_size = 0;
        payload_size += pack_key<INT_32>(buf + payload_size, 1);
        payload_size += pack<INT_32>(buf + payload_size, a);
        payload_size += pack_key<INT_32>(buf + payload_size, 2);
        payload_size += pack<ARRAY>(buf + payload_size, b);
        payload_size += pack_key<INT_32>(buf + payload_size, 3);
        payload_size += pack<MESSAGE>(buf + payload_size, c);
        return payload_size;
    };

    size_t do_unpack(uint8_t* buf, size_t& payload_size){
        uint32_t field_number = 0;
        size_t offset = 0;
        offset += unpack_key<INT_32>(buf + offset, field_number);
        offset += unpack<INT_32>(buf + offset, a);
        offset += unpack_key<INT_32>(buf + offset, field_number);
        offset += unpack<ARRAY>(buf + offset, b);
        offset += unpack_key<INT_32>(buf + offset, field_number);
        offset += unpack<MESSAGE>(buf + offset, c);
        return payload_size;
    };

};

