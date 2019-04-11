#include "controller.hpp"

Controller::Controller()
:producer_key(std::string("EOS6hNSYPHaJm1TBmrLHLytxjwEcX6gbVNRZe4o7DxcVL7ojH9PCZ"))
,private_key(std::string("5HwLWM8J46qbpZDMYsSGDxWN9EkusGwGP1EH5eFJKYvasuW83yX"))
{
}

Controller::~Controller()
{
}

void Controller::commitTrx(const Transaction& trx)
{
    trx_pool.push_back(trx);
}

void Controller::pushBlock()
{
    Block blk;
    if ( blk_cache.size() > 0 ) 
    {
        Block& last_blk = blk_cache.back();
        blk.prior_hash = fc::sha256::hash(last_blk.digest());
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
}