#include "block.hpp"

Block::Block()
{
    time_point = fc::time_point::now();
}

fc::sha256 Block::digest()
{
    fc::sha256::encoder enc;
    fc::raw::pack(enc, blk_num);
    fc::raw::pack(enc, prior_hash);
    fc::raw::pack(enc, time_point);
    fc::raw::pack(enc, merkle_root);
    fc::raw::pack(enc, trxs);
    return enc.result();
}

void Block::build(uint64_t blk_num, fc::sha256 prior_hash, std::vector<Transaction>& trxs)
{
    /*
        uint64_t blk_num;
    fc::sha256  prior_hash;
    fc::time_point_sec time_point;
    fc::sha256 merkle_root;
    fc::sha256 sign;
    std::vector<Transaction> trxs;
    */
    this->trxs.assign(trxs.cbegin(), trxs.cend());
    this->blk_num = blk_num;
    this->prior_hash = prior_hash;

    merkle_root = fc::sha256::hash(trxs);
    //dlog("hash = ${h}",("h", hash));
}