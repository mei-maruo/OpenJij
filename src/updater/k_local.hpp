//    Copyright 2021 Jij Inc.
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at

//        http://www.apache.org/licenses/LICENSE-2.0

//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

#ifndef k_local_hpp
#define k_local_hpp

#include <system/k_local_polynomial.hpp>
#include <utility/schedule_list.hpp>

#include <random>


namespace openjij {
namespace updater {

template<typename System>
struct KLocal;


template<typename GraphType>
struct KLocal<system::KLocalPolynomial<GraphType>> {
  
   using CPIsing = system::KLocalPolynomial<GraphType>;
   
   //! @brief floating point type
   using FloatType = typename GraphType::value_type;
   
   template<typename RandomNumberEngine>
   inline static void update(CPIsing &system,
                             RandomNumberEngine &random_number_engine,
                             const utility::ClassicalUpdaterParameter &parameter
                             ) {
      
      auto urd = std::uniform_real_distribution<>(0, 1.0);

      for (const auto &index_binary: system.get_active_binaries()) {
         const auto dE_s = system.dE_single(index_binary);
         
         if (system.count_call_updater%system.rate_call_k_local == 0 && dE_s == 0.0 && system.binaries[index_binary] == 0) {
            for (const auto &index_key: system.GetAdj(index_binary)) {
               
               if (system.GetPolyValue(index_key) > 0.0) {
                  break;
               }
               
               const auto dE_i = system.dE_k_local(index_key);
               if (dE_i <= 0.0 || std::exp(-parameter.beta*dE_i) > urd(random_number_engine)) {
                  system.update_system_k_local();
               }
               else {
                  system.reset_virtual_system();
                  system.update_system_single(index_binary);
               }
               
            }
            
         }
        else if (dE_s <= 0.0 || std::exp(-parameter.beta*dE_s) > urd(random_number_engine)) {
            system.update_system_single(index_binary);
         }
      }
      
      system.count_call_updater++;
      
   }
       
      
      
      
   
   
};



} // namespace updater
} // namespace openjij


#endif /* k_local_hpp */
