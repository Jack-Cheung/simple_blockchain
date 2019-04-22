/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosio/sqlite_plugin/sqlite_plugin.hpp>

namespace eosio {
   static appbase::abstract_plugin& _sqlite_plugin = app().register_plugin<sqlite_plugin>();

class sqlite_plugin_impl {
   public:
      void db_open();
      void db_close();

      void get_block(uint64_t num);
      //void get_block(fc::sha256 h);
      void append_block(std::string json);
   private:
      static int append_block_callback(void *NotUsed, int argc, char **argv, char **azColName);
   private:
      sqlite3* db;
};

sqlite_plugin::sqlite_plugin():my(new sqlite_plugin_impl()){}
sqlite_plugin::~sqlite_plugin(){}

void sqlite_plugin::set_program_options(options_description&, options_description& cfg) {
   cfg.add_options()
         ("option-name", bpo::value<string>()->default_value("default value"),
          "Option Description")
         ;
}

void sqlite_plugin::plugin_initialize(const variables_map& options) {
   try {
      if( options.count( "option-name" )) {
         // Handle the option
      }
   }
   FC_LOG_AND_RETHROW()
}

void sqlite_plugin::plugin_startup() {
   my->db_open();

}

void sqlite_plugin::plugin_shutdown() {
   // OK, that's enough magic
}

void sqlite_plugin_impl::db_open(){
   int ret = sqlite3_open(DB_PATH, &db);
   if (ret) {
      elog("error happened: ${e}", ("e", sqlite3_errmsg(db)));
   }
}

void sqlite_plugin_impl::db_close(){
   sqlite3_close(db);
}

int sqlite_plugin_impl::append_block_callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      dlog("${n} = ${v}", ("n", azColName[i])("v", argv[i] ? argv[i] : "NULL"));
   }
   return 0;
}

void sqlite_plugin_impl::append_block(std::string json){
   char* zErrMsg = NULL;
   const char* sql = "INSERT INTO BLOCK (ID,HASH,JSON) "  \
         "VALUES(1,'FFFF33','{\"num\":1,\"trx\":[{\"id\":1,\"content\":\"FFF3390\"},{\"id\":2,\"content\":\"EE9876443\"}]}');";
   /* Execute SQL statement */

   /*
   INSERT INTO BLOCK (ID,HASH,JSON) VALUES (1, 'FFFF33', '{"num":1, "trx":[{"id":1, "content":"FFF3390"},{"id":2, "content":"EE9876443"}]}');
   
   
   */
   int rc = sqlite3_exec(db, sql, append_block_callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      elog("SQL error: ${e}\n", ("e", zErrMsg));
      sqlite3_free(zErrMsg);
   }else{
      dlog("Records created successfully");
   }
}

}
