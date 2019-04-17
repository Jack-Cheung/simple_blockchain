/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <appbase/application.hpp>
#include <controller.hpp>

namespace fc{
   FC_DECLARE_EXCEPTION( trx_invalid_arg_exception, invalid_arg_exception_code, "Invalid Argument" );
   //eg."74686973" ====> "this"
   string from_hex(const std::vector<char>& vec);
}

namespace eosio {

using namespace appbase;

template<typename T>
using next_function = std::function<void(const fc::static_variant<fc::exception_ptr, T>&)>;

namespace chain_apis
{
   typedef std::vector<char> bytes;
   struct empty{};
   class read_only
   {
   public:
      struct get_info_results
      {
         std::string info;
      };

      using get_info_params = empty; 
      fc::variant get_info(const get_info_params& p);
      
      using get_block_results = Block;
      struct get_block_params{
         uint64_t blk_num;
      };
      fc::variant get_block(const get_block_params& p);

      read_only(const Controller& c):ctrl(c){};
      ~read_only() = default;
      void validate() const {}
   private:
      const Controller& ctrl;
   };

   class read_write
   {
   public:
      struct push_transaction_params
      {
         uint32_t type;
         fc::crypto::public_key pub_key;
         bytes data;
         bytes attach;
      };
      struct push_transaction_results
      {
         fc::variant res;
      };
      using publish_blk_params = empty;
      void push_transaction(const push_transaction_params& p, next_function<push_transaction_results> next);
      void publish_blk(const publish_blk_params& p, next_function<fc::variant> next);
      read_write(Controller& ctrl):ctrl(ctrl){};
      ~read_write() = default;
      void validate() const {}
   private:
      Controller& ctrl;
   };
}

class chain_plugin : public appbase::plugin<chain_plugin> {
public:
   chain_plugin();
   virtual ~chain_plugin();
 
   APPBASE_PLUGIN_REQUIRES()
   virtual void set_program_options(options_description&, options_description& cfg) override;
 
   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

   const Controller& chain() const;
   Controller& chain();

   chain_apis::read_only get_read_only_api() const { return chain_apis::read_only(chain()); }
   chain_apis::read_write get_read_write_api() { return chain_apis::read_write(chain()); }
private:
   std::unique_ptr<class chain_plugin_impl> my;
};

}

FC_REFLECT(eosio::chain_apis::empty,)
//FC_REFLECT(eosio::chain_apis::read_only::get_info_results, (info))
FC_REFLECT(eosio::chain_apis::read_only::get_block_params, (blk_num))
FC_REFLECT(eosio::chain_apis::read_write::push_transaction_results, (res))
FC_REFLECT(eosio::chain_apis::read_write::push_transaction_params, (type)(data)(attach)(pub_key))
