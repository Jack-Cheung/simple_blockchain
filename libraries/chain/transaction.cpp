#include "transaction.hpp"

fc::sha256 Transaction::digest() const
{
    fc::sha256::encoder  enc;
    fc::raw::pack(enc, time_point);
    fc::raw::pack(enc, attach);
    fc::raw::pack(enc, data);
    fc::raw::pack(enc, pub_key);
    return enc.result();
}

bool Transaction::validate() const
{
    //TODO  it is also needed to examin that if the time is too delay, the program's now time is not right
    if (0 == data.size()) {
        wlog("transaction without data");
        return false;
    }
    if (fc::crypto::signature() == signature) {
        wlog("a blank signature");
        return false;
    }
    auto recover_key = fc::crypto::public_key(signature, digest());
    dlog("recover key = ${rec}, public key = ${pub}", ("rec", recover_key)("pub", pub_key)); 
    return recover_key == pub_key;
}