#pragma once
#include "block.hpp"
#include "transaction.hpp"
#include <fc/io/json.hpp>
class Controller
{
private:
    std::vector<Transaction> trx_pool;
    fc::crypto::public_key producer_key;
    fc::crypto::private_key private_key;
    std::vector<Block> blk_cache;
public:
    Controller();
    ~Controller();
    void commitTrx(const Transaction& trx);
    void pushBlock();
    void replayBlock();
};

