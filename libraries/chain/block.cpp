#include "block.hpp"

Block::Block()
{
    time_point = fc::time_point::now();
}

fc::sha256 Block::digest() const
{
    fc::sha256::encoder enc;
    fc::raw::pack(enc, blk_num);
    fc::raw::pack(enc, prior_hash);
    fc::raw::pack(enc, time_point);
    fc::raw::pack(enc, merkle_root);
    //fc::raw::pack(enc, trxs);
    return enc.result();
}

void Block::push_trxs(std::vector<Transaction>& trxs)
{
    this->trxs.assign(trxs.cbegin(), trxs.cend());
    //FIXME  the merkle root hash may be not right
    merkle_root = fc::sha256::hash(trxs);
}