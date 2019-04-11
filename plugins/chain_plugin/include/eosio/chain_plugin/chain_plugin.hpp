/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <appbase/application.hpp>
#include <controller.hpp>
namespace eosio {

using namespace appbase;

/**
 *  This is a template plugin, intended to serve as a starting point for making new plugins
 */
namespace chain_apis
{
   class read_only
   {
   private:
      /* data */
   public:
      read_only(/* args */);
      ~read_only();
      void validate() const {}
   };

   class read_write
   {
   private:
      /* data */
   public:
      read_write(/* args */);
      ~read_write();
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
