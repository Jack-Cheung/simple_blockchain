/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <appbase/application.hpp>
#include <controller.hpp>
namespace eosio {

using namespace appbase;

template<typename T>
using next_function = std::function<void(const fc::static_variant<fc::exception_ptr, T>&)>;



namespace chain_apis
{
   struct empty{};
   class read_only
   {
   public:
      struct get_info_results
      {
         std::string info;
      };

      using get_info_params = empty; 
      get_info_results get_info(const get_info_params& p);
      
      read_only() = default;
      ~read_only() = default;
      void validate() const {}
   };

   class read_write
   {
   public:
      struct push_transaction_params
      {
         std::string p1;
         std::string p2;
      };

      struct push_transaction_results
      {
         std::string res1;
         std::string res2;
      };

      void push_transaction(const push_transaction_params& p, next_function<push_transaction_results> next);

      read_write() = default;
      ~read_write() = default;
      void validate() const {}
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

   chain_apis::read_only get_read_only_api() const { return chain_apis::read_only(); }
   chain_apis::read_write get_read_write_api() { return chain_apis::read_write(); }
private:
   std::unique_ptr<class chain_plugin_impl> my;
};

}

FC_REFLECT(eosio::chain_apis::empty,)
FC_REFLECT(eosio::chain_apis::read_only::get_info_results, (info))
FC_REFLECT(eosio::chain_apis::read_write::push_transaction_results, (res1)(res2))
FC_REFLECT(eosio::chain_apis::read_write::push_transaction_params, (p1)(p2))
