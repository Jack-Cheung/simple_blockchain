add_subdirectory(mycontract)
set(MYCONTRACT_PRIVATE_KEY 5K5iT9EdUCQbS7uCqa5LVFJSpVE3UDXs1yYXTy1j5FuzEaE4PWV)
set(EOS_DEVELOP_KEY  EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp.ini
 ${CMAKE_CURRENT_SOURCE_DIR}/include/config.hpp ESCAPE_QUOTES)

file(GLOB HEADERS "include/eosio/chain_plugin/*.hpp")
add_library( chain_plugin
             chain_plugin.cpp
             ${HEADERS} )

target_link_libraries( chain_plugin eosio_chain appbase )
target_include_directories( chain_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/../chain_interface/include" "${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/appbase/include")
