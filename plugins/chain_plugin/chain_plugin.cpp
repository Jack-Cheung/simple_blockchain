/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <fc/exception/exception.hpp>

namespace fc
{
   //TODO if not used, remove it
   string from_hex(const std::vector<char>& vec)
   {
      string ret;
      for(auto& e: vec)
      {
         ret += e;
      }
      return ret;
   }
}

namespace eosio
{
static appbase::abstract_plugin &_chain_plugin = app().register_plugin<chain_plugin>();

#define CATCH_AND_CALL(NEXT)                                             \
   catch (const fc::exception &err)                                      \
   {                                                                     \
      NEXT(err.dynamic_copy_exception());                                \
   }                                                                     \
   catch (const std::exception &e)                                       \
   {                                                                     \
      fc::exception fce(                                                 \
          FC_LOG_MESSAGE(warn, "rethrow ${what}: ", ("what", e.what())), \
          fc::std_exception_code,                                        \
          BOOST_CORE_TYPEID(e).name(),                                   \
          e.what());                                                     \
      NEXT(fce.dynamic_copy_exception());                                \
   }                                                                     \
   catch (...)                                                           \
   {                                                                     \
      fc::unhandled_exception e(                                         \
          FC_LOG_MESSAGE(warn, "rethrow"),                               \
          std::current_exception());                                     \
      NEXT(e.dynamic_copy_exception());                                  \
   }

class chain_plugin_impl
{
 public:
   fc::optional<Controller> chain;
};

chain_plugin::chain_plugin() : my(new chain_plugin_impl()) {}
chain_plugin::~chain_plugin() {}

void chain_plugin::set_program_options(options_description &, options_description &cfg)
{
   cfg.add_options()("option-name", bpo::value<string>()->default_value("default value"),
                     "Option Description");
}

void chain_plugin::plugin_initialize(const variables_map &options)
{
   try
   {
      if (options.count("option-name"))
      {
         // Handle the option
      }
      my->chain.emplace();
   }
   FC_LOG_AND_RETHROW()
}

void chain_plugin::plugin_startup()
{
   dlog("chain_plugin startup");
   FC_ASSERT(my->chain, "controller object is nullopt");
   Controller& c = *my->chain;
   /*std::vector<Transaction> trxs;
   Transaction t1;
   t1.fillTestData();
   Transaction t2;
   t2.fillTestData();
   c.commitTrx(t1);
   c.commitTrx(t2);
   c.pushBlock();
   c.commitTrx(t2);
   c.commitTrx(t2);
   c.pushBlock();*/
   sqlite_plugin* db = app().find_plugin<sqlite_plugin>();
   //std::vector<Block> blks = std::move(db->get_all_blocks());
   //dlog("replayed blocks = ${b}", ("b", blks));
   c.replayBlock(db->get_all_blocks());
   //c->replayBlock();
   
}

void chain_plugin::plugin_shutdown()
{
   dlog("chain_plugin shutdown");
}

Controller &chain_plugin::chain() { return *my->chain; }

const Controller &chain_plugin::chain() const { return *my->chain; }

namespace chain_apis
{

fc::variant read_only::get_info(const get_info_params &p)
{
   auto res = ctrl.getInfo();
   return res;
}

fc::variant read_only::get_block(const get_block_params& p)
{
   const auto blk = ctrl.getBlock(p.blk_num);
   if (blk) {
      fc::variant v;
      fc::to_variant(*blk, v);
      return v;
   }
   //TODO get block from database
   return fc::mutable_variant_object()
         ("error", "block not found");
}

TO_REMOVED std::vector<fc::variant> read_only::get_keys(const get_keys_params&){
   using namespace fc::crypto;
   using namespace fc;
   auto key = private_key::generate<ecc::private_key_shim>();
   auto pub = key.get_public_key();
   dlog("PUBLIC KEY: ${pub}\nPRIVATE KEY: ${pri}\n", ("pub", pub)("pri", key));
   variant keyv, pubv;
   to_variant(key, keyv);
   to_variant(pub, pubv);
   std::vector<variant> vec;
   vec.push_back(pubv);
   vec.push_back(keyv);
   return vec;
}

TO_REMOVED std::string read_only::hex2char(const hex2char_params& p){
   using namespace fc::crypto;
   using namespace fc;
   string hex = p.hex;
   char* out_data = new char[1024]{0};
   size_t len = from_hex(hex, out_data, 1024);
   dlog("ASCII CHARS: ${c}\n", ("c", out_data));
   std::string ret = out_data;
   delete[] out_data;
   return ret;
}

TO_REMOVED std::string read_only::char2hex(const char2hex_params& p){
   using namespace fc::crypto;
   using namespace fc;
   std::string str = p.str;
   std::string hex = fc::to_hex(str.c_str(), str.size());
   dlog("HEX CHARS: ${h}\n",("h", hex));
   return hex;
}

TO_REMOVED fc::variant read_only::sign_trx(const sign_trx_params& p){
   using namespace fc::crypto;
   using namespace fc;
   try
   {
      Transaction tx;
      std::string key = p.key;
      from_variant(p.trx, tx);
      dlog("signing trx:${trx}", ("trx", tx));
      private_key pri_key(key);
      dlog("private key user send:${k}", ("k", pri_key));
      tx.pub_key = pri_key.get_public_key();
      sha256 digest = tx.digest();
      dlog("digest :${k}", ("k", digest));
      tx.signature = pri_key.sign(digest);
      variant v;
      to_variant(tx,v);
      dlog("SIGNED TRANSACTION: ${j}\n", ("j", json::to_string(tx)));
      dlog("SIGNED TRANSACTION: ${j}\n", ("j", tx));
      return v;  
   }
   catch(const fc::exception& e)
   {
      std::cerr << e.what() << '\n';
   }
   return fc::variant();
}


void read_write::push_transaction(const read_write::push_transaction_params &p, next_function<push_transaction_results> next)
{
   try
   {
      Transaction trx;
      trx.data = std::move(p.data);
      dlog("p.data=${d}", ("d", p.data));
      trx.attach = std::move(p.attach);
      trx.pub_key = p.pub_key;
      trx.signature = p.signature;
      trx.time_point = p.time_point;
      if (!trx.validate())
      {
         FC_THROW_EXCEPTION(fc::trx_invalid_arg_exception,"transaction is not acceptable");
      }
      ctrl.commitTrx(trx);
      fc::variant v;
      fc::to_variant(trx, v);
      next(push_transaction_results{v});
   }
   CATCH_AND_CALL(next)
}

void read_write::publish_blk(const publish_blk_params& p, next_function<fc::variant> next)
{
   try
   {
      Block last_blk = ctrl.pushBlock();
      sqlite_plugin* plugin = app().find_plugin<sqlite_plugin>();
      plugin->append_block(last_blk);
      next(fc::variant(fc::mutable_variant_object()("message","operation complete")));
   }
   CATCH_AND_CALL(next)
}

} // namespace chain_apis

} // namespace eosio