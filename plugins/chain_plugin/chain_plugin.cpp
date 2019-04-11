/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <fc/exception/exception.hpp>

namespace eosio {
   static appbase::abstract_plugin& _chain_plugin = app().register_plugin<chain_plugin>();

class chain_plugin_impl {
public:

   fc::optional<Controller>         chain;
};

chain_plugin::chain_plugin():my(new chain_plugin_impl()){}
chain_plugin::~chain_plugin(){}

void chain_plugin::set_program_options(options_description&, options_description& cfg) {
   cfg.add_options()
         ("option-name", bpo::value<string>()->default_value("default value"),
          "Option Description")
         ;
}

void chain_plugin::plugin_initialize(const variables_map& options) {
   try {
      if( options.count( "option-name" )) {
         // Handle the option
      }
      my->chain.emplace();
   }
   FC_LOG_AND_RETHROW()
}

void chain_plugin::plugin_startup() {
   dlog("chain_plugin startup");
   Controller  c;
   std::vector<Transaction> trxs;
   Transaction t1;
   t1.fillTestData();
   Transaction t2;
   t2.fillTestData();
   c.commitTrx(t1);
   c.commitTrx(t2);
   c.pushBlock();
   c.commitTrx(t2);
   c.commitTrx(t2);
   c.pushBlock();
}

void chain_plugin::plugin_shutdown() {
   dlog("chain_plugin shutdown");
}

Controller& chain_plugin::chain() { return *my->chain; }

const Controller& chain_plugin::chain() const { return *my->chain; }

}
