#include "controller.hpp"

Controller::Controller()
:producer_key(std::string("EOS6hNSYPHaJm1TBmrLHLytxjwEcX6gbVNRZe4o7DxcVL7ojH9PCZ"))
,private_key(std::string("5HwLWM8J46qbpZDMYsSGDxWN9EkusGwGP1EH5eFJKYvasuW83yX"))
{
}

Controller::~Controller()
{
}

void Controller::commitTrx(Transaction& trx)
{
    //TODO  wrong signature! should be signed at client side and with user's private key
    //trx.time_point = fc::time_point::now();
    //fc::sha256 digest = trx.digest();
    //trx.signature = private_key.sign(digest);
    trx_pool.push_back(trx);
}

Block Controller::pushBlock()
{
    Block blk;
    if ( blk_cache.size() > 0 ) 
    {
        Block& last_blk = blk_cache.back();
        //TODO  not tend to calculate hash's hash
        //blk.prior_hash = fc::sha256::hash(last_blk.digest());
        blk.prior_hash = last_blk.digest();
        blk.blk_num = last_blk.blk_num + 1;
    }
    else
    {
        blk.prior_hash = fc::sha256();
        blk.blk_num = 0;
    }
    blk.push_trxs(trx_pool);
    blk.sig = private_key.sign(blk.digest());
    blk_cache.push_back(blk);
    trx_pool.clear();
    dlog("new block = ${j}",("j", fc::json::to_string(blk)));
    return blk;
}

fc::optional<Block> Controller::getBlock(uint64_t num) const
{
    auto itr = std::find_if(blk_cache.begin(), blk_cache.end(), [num](auto& elem)->bool {
        return elem.blk_num == num;
    });
    if (itr != blk_cache.end()) {
        return *itr;
    }
    return fc::optional<Block>();
}

fc::variant Controller::getInfo() const
{
    return fc::mutable_variant_object()
            ("block_num", blk_cache.size())
            ("pending_trx", trx_pool.size());
}

void Controller::replayBlock(std::vector<Block>&& blocks)
{
    blk_cache = std::forward<std::vector<Block>>(blocks);
}