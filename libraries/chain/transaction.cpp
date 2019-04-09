#include "transaction.hpp"

void Transaction::fillTestData()
{
    time_point = fc::time_point::now();
    data = fc::raw::pack(std::string("this is test data"));
    attach = fc::raw::pack(std::string("this is test attach"));
    fc::crypto::public_key  pub_k;
    fc::crypto::private_key priv_k;
    priv_k = fc::crypto::private_key::generate<fc::ecc::private_key_shim>();
    this->pub_key = priv_k.get_public_key();
    dlog("public_key = ${pubk}, private_key= ${privk}", ("pubk", this->pub_key)("privk", priv_k));
    fc::sha256 dgst = digest();
    dlog("digest=${d}", ("d", dgst));
    signature = priv_k.sign(dgst);
    dlog("signature=${s}", ("s", signature));
    fc::crypto::public_key k = fc::crypto::public_key(signature, dgst);
    dlog("public key from sig=${pk}", ("pk", k));

}

fc::sha256 Transaction::digest()
{
    fc::sha256::encoder  enc;
    fc::raw::pack(enc, time_point);
    fc::raw::pack(enc, attach);
    fc::raw::pack(enc, data);
    fc::raw::pack(enc, pub_key);
    return enc.result();
}