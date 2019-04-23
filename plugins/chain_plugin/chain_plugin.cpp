/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <fc/exception/exception.hpp>

namespace fc
{
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
   return fc::mutable_variant_object()
         ("error", "block not found");
}

void read_write::push_transaction(const read_write::push_transaction_params &p, next_function<push_transaction_results> next)
{
   try
   {
      if (p.type == 0 || p.data.empty())
      {
         FC_THROW_EXCEPTION(fc::trx_invalid_arg_exception,"p1 is empty");
      }
      Transaction trx;
      trx.data = std::move(p.data);
      dlog("p.data=${d}", ("d", p.data));
      trx.attach = std::move(p.attach);
      trx.pub_key = p.pub_key;
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
      ctrl.pushBlock();
      next(fc::variant(fc::mutable_variant_object()("message","operation complete")));
   }
   CATCH_AND_CALL(next)
}

} // namespace chain_apis

} // namespace eosio