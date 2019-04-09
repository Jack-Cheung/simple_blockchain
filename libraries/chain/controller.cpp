#include "controller.hpp"

void Controller::buildBlock()
{

}

Controller::Controller()
:producer_key(std::string("EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV"))
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
    //TEST
    Block blk;
    std::vector<Transaction> trxs;
    Transaction t1;
    t1.fillTestData();
    Transaction t2;
    t2.fillTestData();
    trxs.push_back(t1);
    trxs.push_back(t2);
    blk.build(0, fc::sha256::hash(0), trxs);
    dlog("json = ${j}",("j", fc::json::to_string(blk)));
}